// Based on oneTBB (https://github.com/uxlfoundation/oneTBB)
// See THIRD-PARTY-NOTICES

#pragma once

#include <atomic>
#include <cassert>
#include <cstring>
#include <memory>
#include <type_traits>

#include "ws/concurrency/internal/allocator_traits.h"
#include "ws/concurrency/internal/atomic_backoff.h"
#include "ws/concurrency/internal/helpers.h"

#if defined(_MSC_VER) && !defined(__INTEL_COMPILER)
#pragma warning(push)
#pragma warning(disable : 4127)
#endif

namespace ws {
namespace concurrency {
namespace internal {

template <typename T, typename TAllocator, typename TDerivedType,
          std::size_t PointersPerEmbeddedTable>
class SegmentTable {
 public:
  using value_type = T;
  using segment_type = T*;
  using atomic_segment = std::atomic<segment_type>;
  using segment_table_type = atomic_segment*;

  using size_type = std::size_t;
  using segment_index_type = std::size_t;

  using allocator_type = TAllocator;

  using allocator_traits_type = std::allocator_traits<allocator_type>;
  using segment_table_allocator_type =
      typename allocator_traits_type::template rebind_alloc<atomic_segment>;

 protected:
  using segment_table_allocator_traits =
      std::allocator_traits<segment_table_allocator_type>;
  using derived_type = TDerivedType;

  static constexpr size_type kPointersPerEmbeddedTable =
      PointersPerEmbeddedTable;
  static constexpr size_type kPointersPerLongTable = sizeof(size_type) * 8;

 public:
  SegmentTable(const allocator_type& alloc = allocator_type())
      : segment_table_allocator_(alloc),
        segment_table_(nullptr),
        first_block_{},
        size_{},
        segment_table_allocation_failed_{} {
    segment_table_.store(my_embedded_table_, std::memory_order_relaxed);
    ZeroTable(my_embedded_table_, kPointersPerEmbeddedTable);
  }

  SegmentTable(const SegmentTable& other)
      : segment_table_allocator_(segment_table_allocator_traits::
                                     select_on_container_copy_construction(
                                         other.segment_table_allocator_)),
        segment_table_(nullptr),
        first_block_{},
        size_{},
        segment_table_allocation_failed_{} {
    segment_table_.store(my_embedded_table_, std::memory_order_relaxed);
    ZeroTable(my_embedded_table_, kPointersPerEmbeddedTable);
    ws::concurrency::internal::templates::TryCall([&] {
      InternalTransfer(other, CopySegmentBodyType{*this});
    }).OnException([&] { Clear(); });
  }

  SegmentTable(const SegmentTable& other, const allocator_type& alloc)
      : segment_table_allocator_(alloc),
        segment_table_(nullptr),
        first_block_{},
        size_{},
        segment_table_allocation_failed_{} {
    segment_table_.store(my_embedded_table_, std::memory_order_relaxed);
    ZeroTable(my_embedded_table_, kPointersPerEmbeddedTable);
    TryCall([&] {
      InternalTransfer(other, CopySegmentBodyType{*this});
    }).OnException([&] { Clear(); });
  }

  SegmentTable(SegmentTable&& other)
      : segment_table_allocator_(std::move(other.segment_table_allocator_)),
        segment_table_(nullptr),
        first_block_{},
        size_{},
        segment_table_allocation_failed_{} {
    segment_table_.store(my_embedded_table_, std::memory_order_relaxed);
    ZeroTable(my_embedded_table_, kPointersPerEmbeddedTable);
    InternalMove(std::move(other));
  }

  SegmentTable(SegmentTable&& other, const allocator_type& alloc)
      : segment_table_allocator_(alloc),
        segment_table_(nullptr),
        first_block_{},
        size_{},
        segment_table_allocation_failed_{} {
    segment_table_.store(my_embedded_table_, std::memory_order_relaxed);
    ZeroTable(my_embedded_table_, kPointersPerEmbeddedTable);
    using is_equal_type =
        typename segment_table_allocator_traits::is_always_equal;
    InternalMoveConstructWithAllocator(std::move(other), alloc,
                                       is_equal_type());
  }

  ~SegmentTable() { Clear(); }

  SegmentTable& operator=(const SegmentTable& other) {
    if (this != &other) {
      CopyAssignAllocators(segment_table_allocator_,
                           other.segment_table_allocator_);
      InternalTransfer(other, CopySegmentBodyType{*this});
    }

    return *this;
  }

  SegmentTable& operator=(SegmentTable&& other) noexcept(
      derived_type::kIsNoExceptAssignment) {
    using pocma_type = typename segment_table_allocator_traits::
        propagate_on_container_move_assignment;
    using is_equal_type =
        typename segment_table_allocator_traits::is_always_equal;

    if (this != &other) {
      MoveAssignAllocators(segment_table_allocator_,
                           other.segment_table_allocator_);
      InternalMoveAssign(std::move(other),
                         std::disjunction<is_equal_type, pocma_type>());
    }
    return *this;
  }

  void Swap(SegmentTable& other) noexcept(derived_type::kIsNoExceptSwap) {
    using is_equal_type =
        typename segment_table_allocator_traits::is_always_equal;
    using pocs_type =
        typename segment_table_allocator_traits::propagate_on_container_swap;

    if (this != &other) {
      SwapAllocators(segment_table_allocator_, other.segment_table_allocator_);
      InternalSwap(other, std::disjunction<is_equal_type, pocs_type>());
    }
  }

  segment_type GetSegment(segment_index_type index) const {
    return Table()[index] + SegmentBase(index);
  }

  value_type& operator[](size_type index) {
    return InternalSubscript<true>(index);
  }

  const value_type& operator[](size_type index) const {
    return const_cast<SegmentTable*>(this)->InternalSubscript<true>(index);
  }

  const segment_table_allocator_type& get_allocator() const {
    return segment_table_allocator_;
  }

  segment_table_allocator_type& get_allocator() {
    return segment_table_allocator_;
  }

  void EnableSegment(segment_type& segment, segment_table_type table,
                     segment_index_type seg_index, size_type index) {
    segment_type new_segment = Self()->CreateSegment(table, seg_index, index);
    if (new_segment != nullptr) {
      segment_type disabled_segment = nullptr;
      if (!table[seg_index].compare_exchange_strong(
              disabled_segment, new_segment - SegmentBase(seg_index))) {
        Self()->DeallocateSegment(new_segment, seg_index);
      }
    }

    segment = table[seg_index].load(std::memory_order_acquire);
    assert(segment != nullptr &&
           "If CreateSegment returned nullptr, the element should be "
           "stored in the table");
  }

  void DeleteSegment(segment_index_type seg_index) {
    segment_type segment_to_delete = Self()->NullifySegment(Table(), seg_index);
    if (segment_to_delete == kSegmentAllocationFailureTag) {
      return;
    }

    segment_to_delete += SegmentBase(seg_index);

    Self()->DestroySegment(segment_to_delete, seg_index);
  }

  size_type NumberOfSegments(segment_table_type table) const {
    return table == my_embedded_table_ ? kPointersPerEmbeddedTable
                                       : kPointersPerLongTable;
  }

  size_type Capacity() const noexcept {
    segment_table_type table = Table();
    size_type num_segments = NumberOfSegments(table);
    for (size_type seg_index = 0; seg_index < num_segments; ++seg_index) {
      if (table[seg_index].load(std::memory_order_relaxed) <=
          kSegmentAllocationFailureTag) {
        return SegmentBase(seg_index);
      }
    }
    return SegmentBase(num_segments);
  }

  size_type FindLastAllocatedSegment(segment_table_type table) const noexcept {
    size_type end = 0;
    size_type num_segments = NumberOfSegments(table);
    for (size_type seg_index = 0; seg_index < num_segments; ++seg_index) {
      if (table[seg_index].load(std::memory_order_relaxed) >
          kSegmentAllocationFailureTag) {
        end = seg_index + 1;
      }
    }
    return end;
  }

  void Reserve(size_type n) {
    if (n > allocator_traits_type::max_size(segment_table_allocator_))
      throw std::bad_alloc();

    size_type size = size_.load(std::memory_order_relaxed);
    segment_index_type start_seg_idx =
        size == 0 ? 0 : SegmentIndexOf(size - 1) + 1;
    for (segment_index_type seg_idx = start_seg_idx; SegmentBase(seg_idx) < n;
         ++seg_idx) {
      size_type first_index = SegmentBase(seg_idx);
      InternalSubscript<true>(first_index);
    }
  }

  void Clear() {
    ClearSegments();
    ClearTable();
    size_.store(0, std::memory_order_relaxed);
    first_block_.store(0, std::memory_order_relaxed);
  }

  void ClearSegments() {
    segment_table_type current_segment_table = Table();
    for (size_type i = NumberOfSegments(current_segment_table); i != 0; --i) {
      if (current_segment_table[i - 1].load(std::memory_order_relaxed) !=
          nullptr) {
        DeleteSegment(i - 1);
      }
    }
  }

  void DestroyAndDeallocateTable(segment_table_type table,
                                 size_type num_segments) {
    auto& alloc = get_allocator();
    for (size_type seg_idx = 0; seg_idx < num_segments; ++seg_idx) {
      segment_table_allocator_traits::destroy(alloc, &table[seg_idx]);
    }
    segment_table_allocator_traits::deallocate(alloc, table, num_segments);
  }

  void ClearTable() {
    segment_table_type current_segment_table = Table();
    if (current_segment_table != my_embedded_table_) {
      DestroyAndDeallocateTable(current_segment_table, kPointersPerLongTable);
      segment_table_.store(my_embedded_table_, std::memory_order_relaxed);
      ZeroTable(my_embedded_table_, kPointersPerEmbeddedTable);
    }
  }

  void ExtendTableIfNecessary(segment_table_type& table, size_type start_index,
                              size_type end_index) {
    if (table == my_embedded_table_ && end_index > kEmbeddedTableSize) {
      if (start_index <= kEmbeddedTableSize) {
        ws::concurrency::internal::templates::TryCall([&] {
          segment_table_type new_table =
              Self()->AllocateLongTable(my_embedded_table_, start_index);

          if (segment_table_.compare_exchange_strong(
                  table, new_table,
                  /*memory order in case of a success*/
                  std::memory_order_release,
                  /*memory order in case of a failure*/
                  std::memory_order_acquire)) {
            table = new_table;
          } else if (new_table) {
            DestroyAndDeallocateTable(new_table, kPointersPerLongTable);
          }
        }).OnException([&] {
          segment_table_allocation_failed_.store(true,
                                                 std::memory_order_relaxed);
        });
      } else {
        AtomicBackoff backoff;
        do {
          if (segment_table_allocation_failed_.load(std::memory_order_relaxed))
            throw std::bad_alloc();
          ;

          backoff.Wait();
          table = segment_table_.load(std::memory_order_acquire);
        } while (table == my_embedded_table_);
      }
    }
  }

  static constexpr segment_index_type SegmentIndexOf(size_type index) {
    return size_type(ws::concurrency::internal::Log2(uintptr_t(index | 1)));
  }

  static constexpr size_type SegmentBase(size_type index) {
    return size_type(1) << index & ~size_type(1);
  }

  static constexpr size_type SegmentSize(size_type index) {
    return index == 0 ? 2 : size_type(1) << index;
  }

 private:
  derived_type* Self() { return static_cast<derived_type*>(this); }

  struct CopySegmentBodyType {
    void operator()(segment_index_type index, segment_type from,
                    segment_type to) const {
      instance_.Self()->CopySegment(index, from, to);
    }
    SegmentTable& instance_;
  };

  struct MoveSegmentBodyType {
    void operator()(segment_index_type index, segment_type from,
                    segment_type to) const {
      instance_.Self()->MoveSegment(index, from, to);
    }
    SegmentTable& instance_;
  };

  template <typename TTransferBody>
  void InternalTransfer(const SegmentTable& other,
                        TTransferBody transfer_segment) {
    static_cast<derived_type*>(this)->DestroyElements();

    AssignFirstBlockIfNecessary(
        other.first_block_.load(std::memory_order_relaxed));
    size_.store(other.size_.load(std::memory_order_relaxed),
                std::memory_order_relaxed);

    segment_table_type other_table = other.Table();
    size_type end_SegmentSize =
        SegmentSize(other.FindLastAllocatedSegment(other_table));

    size_type other_size =
        end_SegmentSize < other.size_.load(std::memory_order_relaxed)
            ? other.size_.load(std::memory_order_relaxed)
            : end_SegmentSize;
    other_size =
        segment_table_allocation_failed_ ? kEmbeddedTableSize : other_size;

    for (segment_index_type i = 0; SegmentBase(i) < other_size; ++i) {
      if (other_table[i].load(std::memory_order_relaxed) ==
          kSegmentAllocationFailureTag) {
        size_ = SegmentBase(i);
        break;
      } else if (other_table[i].load(std::memory_order_relaxed) != nullptr) {
        InternalSubscript<true>(SegmentBase(i));
        transfer_segment(
            i,
            other.Table()[i].load(std::memory_order_relaxed) + SegmentBase(i),
            Table()[i].load(std::memory_order_relaxed) + SegmentBase(i));
      }
    }
  }

  void InternalMove(SegmentTable&& other) {
    Clear();
    first_block_.store(other.first_block_.load(std::memory_order_relaxed),
                       std::memory_order_relaxed);
    size_.store(other.size_.load(std::memory_order_relaxed),
                std::memory_order_relaxed);

    if (other.Table() == other.my_embedded_table_) {
      for (size_type i = 0; i != kPointersPerEmbeddedTable; ++i) {
        segment_type other_segment =
            other.my_embedded_table_[i].load(std::memory_order_relaxed);
        my_embedded_table_[i].store(other_segment, std::memory_order_relaxed);
        other.my_embedded_table_[i].store(nullptr, std::memory_order_relaxed);
      }
      segment_table_.store(my_embedded_table_, std::memory_order_relaxed);
    } else {
      segment_table_.store(other.segment_table_, std::memory_order_relaxed);
      other.segment_table_.store(other.my_embedded_table_,
                                 std::memory_order_relaxed);
      ZeroTable(other.my_embedded_table_, kPointersPerEmbeddedTable);
    }
    other.size_.store(0, std::memory_order_relaxed);
  }

  void InternalMoveConstructWithAllocator(
      SegmentTable&& other, const allocator_type&,
      /*is_always_equal = */ std::true_type) {
    InternalMove(std::move(other));
  }

  void InternalMoveConstructWithAllocator(
      SegmentTable&& other, const allocator_type& alloc,
      /*is_always_equal = */ std::false_type) {
    if (other.segment_table_allocator_ == alloc) {
      InternalMove(std::move(other));
    } else {
      TryCall([&] {
        InternalTransfer(other, MoveSegmentBodyType{*this});
      }).OnException([&] { Clear(); });
    }
  }

  void InternalMoveAssign(SegmentTable&& other,
                          /*is_always_equal || POCMA = */ std::true_type) {
    InternalMove(std::move(other));
  }

  void InternalMoveAssign(SegmentTable&& other,
                          /*is_always_equal || POCMA = */ std::false_type) {
    if (segment_table_allocator_ == other.segment_table_allocator_) {
      InternalMove(std::move(other));
    } else {
      InternalTransfer(other, MoveSegmentBodyType{*this});
    }
  }

  void InternalSwap(SegmentTable& other,
                    /*is_always_equal || POCS = */ std::true_type) {
    InternalSwapFields(other);
  }

  void InternalSwap(SegmentTable& other,
                    /*is_always_equal || POCS = */ std::false_type) {
    assert(segment_table_allocator_ == other.segment_table_allocator_ &&
           "Swapping with unequal allocators is not allowed");
    InternalSwapFields(other);
  }

  void InternalSwapFields(SegmentTable& other) {
    if (Table() == my_embedded_table_ ||
        other.Table() == other.my_embedded_table_) {
      for (size_type i = 0; i != kPointersPerEmbeddedTable; ++i) {
        segment_type current_segment =
            my_embedded_table_[i].load(std::memory_order_relaxed);
        segment_type other_segment =
            other.my_embedded_table_[i].load(std::memory_order_relaxed);

        my_embedded_table_[i].store(other_segment, std::memory_order_relaxed);
        other.my_embedded_table_[i].store(current_segment,
                                          std::memory_order_relaxed);
      }
    }

    segment_table_type current_segment_table = Table();
    segment_table_type other_segment_table = other.Table();

    if (current_segment_table == my_embedded_table_) {
      other.segment_table_.store(other.my_embedded_table_,
                                 std::memory_order_relaxed);
    } else {
      other.segment_table_.store(current_segment_table,
                                 std::memory_order_relaxed);
    }

    if (other_segment_table == other.my_embedded_table_) {
      segment_table_.store(my_embedded_table_, std::memory_order_relaxed);
    } else {
      segment_table_.store(other_segment_table, std::memory_order_relaxed);
    }
    auto first_block = other.first_block_.load(std::memory_order_relaxed);
    other.first_block_.store(first_block_.load(std::memory_order_relaxed),
                             std::memory_order_relaxed);
    first_block_.store(first_block, std::memory_order_relaxed);

    auto size = other.size_.load(std::memory_order_relaxed);
    other.size_.store(size_.load(std::memory_order_relaxed),
                      std::memory_order_relaxed);
    size_.store(size, std::memory_order_relaxed);
  }

 protected:
  const segment_type kSegmentAllocationFailureTag =
      reinterpret_cast<segment_type>(1);
  static constexpr size_type kEmbeddedTableSize =
      SegmentSize(kPointersPerEmbeddedTable);

  template <bool allow_out_of_range_access>
  value_type& InternalSubscript(size_type index) {
    segment_index_type seg_index = SegmentIndexOf(index);
    segment_table_type table = segment_table_.load(std::memory_order_acquire);
    segment_type segment = nullptr;

    if (allow_out_of_range_access) {
      if (derived_type::kAllowTableExtending) {
        ExtendTableIfNecessary(table, index, index + 1);
      }

      segment = table[seg_index].load(std::memory_order_acquire);

      if (segment == nullptr) {
        EnableSegment(segment, table, seg_index, index);
      }

      if (segment == kSegmentAllocationFailureTag) throw std::bad_alloc();
      ;
    } else {
      segment = table[seg_index].load(std::memory_order_acquire);
    }
    assert(segment != nullptr && "Segment should be enabled");

    return segment[index];
  }

  void AssignFirstBlockIfNecessary(segment_index_type index) {
    size_type zero = 0;
    if (first_block_.load(std::memory_order_relaxed) == zero) {
      first_block_.compare_exchange_strong(zero, index);
    }
  }

  void ZeroTable(segment_table_type table, size_type count) {
    for (size_type i = 0; i != count; ++i) {
      table[i].store(nullptr, std::memory_order_relaxed);
    }
  }

  segment_table_type Table() const {
    return segment_table_.load(std::memory_order_acquire);
  }

  segment_table_allocator_type segment_table_allocator_;
  std::atomic<segment_table_type> segment_table_;
  atomic_segment my_embedded_table_[kPointersPerEmbeddedTable];

  std::atomic<size_type> first_block_;

  std::atomic<size_type> size_;

  std::atomic<bool> segment_table_allocation_failed_;
};

}  // namespace internal
}  // namespace concurrency
}  // namespace ws

#if defined(_MSC_VER) && !defined(__INTEL_COMPILER)
#pragma warning(pop)
#endif
