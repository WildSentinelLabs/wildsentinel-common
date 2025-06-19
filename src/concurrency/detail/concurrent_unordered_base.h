#pragma once

#include <algorithm>
#include <atomic>
#include <initializer_list>
#include <iterator>
#include <memory>
#include <type_traits>
#include <utility>

#include "concurrency/detail/allocator_traits.h"
#include "concurrency/detail/hash_compare.h"
#include "concurrency/detail/helpers.h"
#include "concurrency/detail/node_handle.h"
#include "concurrency/detail/segment_table.h"
#include "config.h"

#if defined(_MSC_VER) && !defined(__INTEL_COMPILER)
#pragma warning(push)
#pragma warning(disable : 4127)
#endif

namespace ws {
namespace concurrency {
namespace detail {

template <typename TTraits>
class ConcurrentUnorderedBase;

template <typename TContainer, typename TValue>
class SolistIterator {
 private:
  using node_ptr = typename TContainer::value_node_ptr;
  template <typename T, typename TAllocator>
  friend class SplitOrderedList;
  template <typename TM, typename TV>
  friend class SolistIterator;
  template <typename TTraits>
  friend class ConcurrentUnorderedBase;
  template <typename TM, typename T, typename TU>
  friend bool operator==(const SolistIterator<TM, T>& i,
                         const SolistIterator<TM, TU>& j);
  template <typename TM, typename T, typename TU>
  friend bool operator!=(const SolistIterator<TM, T>& i,
                         const SolistIterator<TM, TU>& j);

 public:
  using value_type = TValue;
  using difference_type = typename TContainer::difference_type;
  using pointer = value_type*;
  using reference = value_type&;
  using iterator_category = std::forward_iterator_tag;

  SolistIterator() : node_ptr_(nullptr) {}
  SolistIterator(
      const SolistIterator<TContainer, typename TContainer::value_type>& other)
      : node_ptr_(other.node_ptr_) {}

  SolistIterator& operator=(
      const SolistIterator<TContainer, typename TContainer::value_type>&
          other) {
    node_ptr_ = other.node_ptr_;
    return *this;
  }

  reference operator*() const { return node_ptr_->Value(); }

  pointer operator->() const { return node_ptr_->Storage(); }

  SolistIterator& operator++() {
    auto next_node = node_ptr_->Next();
    while (next_node && next_node->IsDummy()) {
      next_node = next_node->Next();
    }
    node_ptr_ = static_cast<node_ptr>(next_node);
    return *this;
  }

  SolistIterator operator++(int) {
    SolistIterator tmp = *this;
    ++*this;
    return tmp;
  }

 private:
  SolistIterator(node_ptr pnode) : node_ptr_(pnode) {}

  node_ptr NodePtr() const { return node_ptr_; }

  node_ptr node_ptr_;
};

template <typename TSolist, typename T, typename TU>
bool operator==(const SolistIterator<TSolist, T>& i,
                const SolistIterator<TSolist, TU>& j) {
  return i.node_ptr_ == j.node_ptr_;
}

template <typename TSolist, typename T, typename TU>
bool operator!=(const SolistIterator<TSolist, T>& i,
                const SolistIterator<TSolist, TU>& j) {
  return i.node_ptr_ != j.node_ptr_;
}

template <typename TSokeyType>
class ListNode {
 public:
  using node_ptr = ListNode*;
  using sokey_type = TSokeyType;

  ListNode(sokey_type key) : next_(nullptr), order_key_(key) {}

  void Init(sokey_type key) { order_key_ = key; }

  sokey_type OrderKey() const { return order_key_; }

  bool IsDummy() { return (order_key_ & 0x1) == 0; }

  node_ptr Next() const { return next_.load(std::memory_order_acquire); }

  void SetNext(node_ptr next_node) {
    next_.store(next_node, std::memory_order_release);
  }

  bool TrySetNext(node_ptr expected_next, node_ptr new_next) {
    return next_.compare_exchange_strong(expected_next, new_next);
  }

 private:
  std::atomic<node_ptr> next_;
  sokey_type order_key_;
};

template <typename TValueType, typename TSokeyType>
class ValueNode : public ListNode<TSokeyType> {
 public:
  using base_type = ListNode<TSokeyType>;
  using sokey_type = typename base_type::sokey_type;
  using value_type = TValueType;

  ValueNode(sokey_type ord_key) : base_type(ord_key) {}
  ~ValueNode() {}
  value_type* Storage() { return &value_; }

  value_type& Value() { return *Storage(); }

 private:
  union {
    value_type value_;
  };
};

template <typename TTraits>
class ConcurrentUnorderedBase {
  using self_type = ConcurrentUnorderedBase<TTraits>;
  using traits_type = TTraits;
  using hash_compare_type = typename traits_type::hash_compare_type;
  class UnorderedSegmentTable;

 public:
  using value_type = typename traits_type::value_type;
  using key_type = typename traits_type::key_type;
  using allocator_type = typename traits_type::allocator_type;

 private:
  using allocator_traits_type = std::allocator_traits<allocator_type>;

  static_assert(
      std::is_same<typename allocator_traits_type::value_type,
                   value_type>::value,
      "value_type of the container must be the same as its allocator");
  using sokey_type = std::size_t;

 public:
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

  using iterator = SolistIterator<self_type, value_type>;
  using const_iterator = SolistIterator<self_type, const value_type>;
  using local_iterator = iterator;
  using const_local_iterator = const_iterator;

  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = typename allocator_traits_type::pointer;
  using const_pointer = typename allocator_traits_type::const_pointer;

  using hasher = typename hash_compare_type::hasher;
  using key_equal = typename hash_compare_type::key_equal;

 private:
  using list_node_type = ListNode<sokey_type>;
  using value_node_type = ValueNode<value_type, sokey_type>;
  using node_ptr = list_node_type*;
  using value_node_ptr = value_node_type*;

  using value_node_allocator_type =
      typename allocator_traits_type::template rebind_alloc<value_node_type>;
  using node_allocator_type =
      typename allocator_traits_type::template rebind_alloc<list_node_type>;

  using node_allocator_traits = std::allocator_traits<node_allocator_type>;
  using value_node_allocator_traits =
      std::allocator_traits<value_node_allocator_type>;

  static constexpr size_type RoundUpToPowerOfTwo(size_type bucket_count) {
    return size_type(1) << size_type(ws::concurrency::detail::Log2(
               uintptr_t(bucket_count == 0 ? 1 : bucket_count) * 2 - 1));
  }

  template <typename T>
  using is_transparent = ws::concurrency::detail::containers::DependentBool<
      ws::concurrency::detail::containers::HasTransparentKeyEqual<
          key_type, hasher, key_equal>,
      T>;

 public:
  using node_type =
      ws::concurrency::detail::NodeHandle<key_type, value_type, value_node_type,
                                          allocator_type>;

  explicit ConcurrentUnorderedBase(
      size_type bucket_count, const hasher& hash = hasher(),
      const key_equal& equal = key_equal(),
      const allocator_type& alloc = allocator_type())
      : size_(0),
        bucket_count_(RoundUpToPowerOfTwo(bucket_count)),
        max_load_factor_(float(kinitialMaxLoadFactor)),
        hash_compare_(hash, equal),
        head_(sokey_type(0)),
        segments_(alloc) {}

  ConcurrentUnorderedBase() : ConcurrentUnorderedBase(kInitialBucketCount) {}

  ConcurrentUnorderedBase(size_type bucket_count, const allocator_type& alloc)
      : ConcurrentUnorderedBase(bucket_count, hasher(), key_equal(), alloc) {}

  ConcurrentUnorderedBase(size_type bucket_count, const hasher& hash,
                          const allocator_type& alloc)
      : ConcurrentUnorderedBase(bucket_count, hash, key_equal(), alloc) {}

  explicit ConcurrentUnorderedBase(const allocator_type& alloc)
      : ConcurrentUnorderedBase(kInitialBucketCount, hasher(), key_equal(),
                                alloc) {}

  template <typename TInputIterator>
  ConcurrentUnorderedBase(TInputIterator first, TInputIterator last,
                          size_type bucket_count = kInitialBucketCount,
                          const hasher& hash = hasher(),
                          const key_equal& equal = key_equal(),
                          const allocator_type& alloc = allocator_type())
      : ConcurrentUnorderedBase(bucket_count, hash, equal, alloc) {
    Insert(first, last);
  }

  template <typename TInputIterator>
  ConcurrentUnorderedBase(TInputIterator first, TInputIterator last,
                          size_type bucket_count, const allocator_type& alloc)
      : ConcurrentUnorderedBase(first, last, bucket_count, hasher(),
                                key_equal(), alloc) {}

  template <typename TInputIterator>
  ConcurrentUnorderedBase(TInputIterator first, TInputIterator last,
                          size_type bucket_count, const hasher& hash,
                          const allocator_type& alloc)
      : ConcurrentUnorderedBase(first, last, bucket_count, hash, key_equal(),
                                alloc) {}

  ConcurrentUnorderedBase(const ConcurrentUnorderedBase& other)
      : size_(other.size_.load(std::memory_order_relaxed)),
        bucket_count_(other.bucket_count_.load(std::memory_order_relaxed)),
        max_load_factor_(other.max_load_factor_),
        hash_compare_(other.hash_compare_),
        head_(other.head_.OrderKey()),
        segments_(other.segments_) {
    ws::concurrency::detail::templates::TryCall([&] {
      InternalCopy(other);
    }).OnException([&] { Clear(); });
  }

  ConcurrentUnorderedBase(const ConcurrentUnorderedBase& other,
                          const allocator_type& alloc)
      : size_(other.size_.load(std::memory_order_relaxed)),
        bucket_count_(other.bucket_count_.load(std::memory_order_relaxed)),
        max_load_factor_(other.max_load_factor_),
        hash_compare_(other.hash_compare_),
        head_(other.head_.OrderKey()),
        segments_(other.segments_, alloc) {
    ws::concurrency::detail::templates::TryCall([&] {
      InternalCopy(other);
    }).OnException([&] { Clear(); });
  }

  ConcurrentUnorderedBase(ConcurrentUnorderedBase&& other)
      : size_(other.size_.load(std::memory_order_relaxed)),
        bucket_count_(other.bucket_count_.load(std::memory_order_relaxed)),
        max_load_factor_(std::move(other.max_load_factor_)),
        hash_compare_(std::move(other.hash_compare_)),
        head_(other.head_.OrderKey()),
        segments_(std::move(other.segments_)) {
    MoveContent(std::move(other));
  }

  ConcurrentUnorderedBase(ConcurrentUnorderedBase&& other,
                          const allocator_type& alloc)
      : size_(other.size_.load(std::memory_order_relaxed)),
        bucket_count_(other.bucket_count_.load(std::memory_order_relaxed)),
        max_load_factor_(std::move(other.max_load_factor_)),
        hash_compare_(std::move(other.hash_compare_)),
        head_(other.head_.OrderKey()),
        segments_(std::move(other.segments_), alloc) {
    using is_always_equal = typename allocator_traits_type::is_always_equal;
    InternalMoveConstructWithAllocator(std::move(other), alloc,
                                       is_always_equal());
  }

  ConcurrentUnorderedBase(std::initializer_list<value_type> init,
                          size_type bucket_count = kInitialBucketCount,
                          const hasher& hash = hasher(),
                          const key_equal& equal = key_equal(),
                          const allocator_type& alloc = allocator_type())
      : ConcurrentUnorderedBase(init.begin(), init.end(), bucket_count, hash,
                                equal, alloc) {}

  ConcurrentUnorderedBase(std::initializer_list<value_type> init,
                          size_type bucket_count, const allocator_type& alloc)
      : ConcurrentUnorderedBase(init, bucket_count, hasher(), key_equal(),
                                alloc) {}

  ConcurrentUnorderedBase(std::initializer_list<value_type> init,
                          size_type bucket_count, const hasher& hash,
                          const allocator_type& alloc)
      : ConcurrentUnorderedBase(init, bucket_count, hash, key_equal(), alloc) {}

  ~ConcurrentUnorderedBase() { InternalClear(); }

  ConcurrentUnorderedBase& operator=(const ConcurrentUnorderedBase& other) {
    if (this != &other) {
      Clear();
      size_.store(other.size_.load(std::memory_order_relaxed),
                  std::memory_order_relaxed);
      bucket_count_.store(other.bucket_count_.load(std::memory_order_relaxed),
                          std::memory_order_relaxed);
      max_load_factor_ = other.max_load_factor_;
      hash_compare_ = other.hash_compare_;
      segments_ = other.segments_;
      InternalCopy(other);
    }
    return *this;
  }

  ConcurrentUnorderedBase& operator=(ConcurrentUnorderedBase&& other) noexcept(
      UnorderedSegmentTable::kIsNoExceptAssignment) {
    if (this != &other) {
      Clear();
      size_.store(other.size_.load(std::memory_order_relaxed),
                  std::memory_order_relaxed);
      bucket_count_.store(other.bucket_count_.load(std::memory_order_relaxed),
                          std::memory_order_relaxed);
      max_load_factor_ = std::move(other.max_load_factor_);
      hash_compare_ = std::move(other.hash_compare_);
      segments_ = std::move(other.segments_);

      using pocma_type = typename allocator_traits_type::
          propagate_on_container_move_assignment;
      using is_always_equal = typename allocator_traits_type::is_always_equal;
      InternalMoveAssign(std::move(other),
                         std::disjunction<pocma_type, is_always_equal>());
    }
    return *this;
  }

  ConcurrentUnorderedBase& operator=(std::initializer_list<value_type> init) {
    Clear();
    Insert(init);
    return *this;
  }

  void Swap(ConcurrentUnorderedBase& other) noexcept(
      UnorderedSegmentTable::kIsNoExceptSwap) {
    if (this != &other) {
      using pocs_type =
          typename allocator_traits_type::propagate_on_container_swap;
      using is_always_equal = typename allocator_traits_type::is_always_equal;
      InternalSwap(other, std::disjunction<pocs_type, is_always_equal>());
    }
  }

  [[nodiscard]] bool Empty() const noexcept { return Size() == 0; }

  allocator_type get_allocator() const noexcept {
    return segments_.get_allocator();
  }

  iterator begin() noexcept { return iterator(FirstValueNode(&head_)); }
  const_iterator begin() const noexcept {
    return const_iterator(FirstValueNode(const_cast<node_ptr>(&head_)));
  }
  const_iterator cbegin() const noexcept {
    return const_iterator(FirstValueNode(const_cast<node_ptr>(&head_)));
  }

  iterator end() noexcept { return iterator(nullptr); }
  const_iterator end() const noexcept { return const_iterator(nullptr); }
  const_iterator cend() const noexcept { return const_iterator(nullptr); }

  size_type Size() const noexcept {
    return size_.load(std::memory_order_relaxed);
  }
  size_type MaxSize() const noexcept {
    return allocator_traits_type::max_size(get_allocator());
  }

  void Clear() noexcept { InternalClear(); }

  std::pair<iterator, bool> Insert(const value_type& value) {
    return InternalInsertValue(value);
  }

  std::pair<iterator, bool> Insert(value_type&& value) {
    return InternalInsertValue(std::move(value));
  }

  iterator Insert(const_iterator, const value_type& value) {
    return Insert(value).first;
  }

  iterator Insert(const_iterator, value_type&& value) {
    return Insert(std::move(value)).first;
  }

  template <typename TInputIterator>
  void Insert(TInputIterator first, TInputIterator last) {
    for (; first != last; ++first) {
      Insert(*first);
    }
  }

  void Insert(std::initializer_list<value_type> init) {
    Insert(init.begin(), init.end());
  }

  std::pair<iterator, bool> Insert(node_type&& nh) {
    if (!nh.Empty()) {
      value_node_ptr insert_node =
          ws::concurrency::detail::NodeHandleAccessor::GetNodePtr(nh);
      auto init_node = [&insert_node](sokey_type order_key) -> value_node_ptr {
        insert_node->Init(order_key);
        return insert_node;
      };
      auto insert_result = InternalInsert(insert_node->Value(), init_node);
      if (insert_result.inserted) {
        assert(insert_result.remaining_node == nullptr &&
               "internal_insert_node should not return the remaining "
               "node if the insertion succeeded");
        ws::concurrency::detail::NodeHandleAccessor::Deactivate(nh);
      }
      return {iterator(insert_result.node_with_equal_key),
              insert_result.inserted};
    }
    return {end(), false};
  }

  iterator Insert(const_iterator, node_type&& nh) {
    return Insert(std::move(nh)).first;
  }

  template <typename... TArgs>
  std::pair<iterator, bool> Emplace(TArgs&&... args) {
    value_node_ptr insert_node = CreateNode(0, std::forward<TArgs>(args)...);

    auto init_node = [&insert_node](sokey_type order_key) -> value_node_ptr {
      insert_node->Init(order_key);
      return insert_node;
    };

    auto insert_result = InternalInsert(insert_node->Value(), init_node);

    if (!insert_result.inserted) {
      insert_node->Init(SplitOrderKeyRegular(1));
      DestroyNode(insert_node);
    }

    return {iterator(insert_result.node_with_equal_key),
            insert_result.inserted};
  }

  template <typename... TArgs>
  iterator EmplaceHint(const_iterator, TArgs&&... args) {
    return Emplace(std::forward<TArgs>(args)...).first;
  }

  iterator UnsafeErase(const_iterator pos) {
    return iterator(FirstValueNode(InternalErase(pos.NodePtr())));
  }

  iterator UnsafeErase(iterator pos) {
    return iterator(FirstValueNode(InternalErase(pos.NodePtr())));
  }

  iterator UnsafeErase(const_iterator first, const_iterator last) {
    while (first != last) {
      first = UnsafeErase(first);
    }
    return iterator(first.NodePtr());
  }

  size_type UnsafeErase(const key_type& key) { return InternalEraseByKey(key); }

  template <typename TK>
  typename std::enable_if<is_transparent<TK>::value &&
                              !std::is_convertible<TK, const_iterator>::value &&
                              !std::is_convertible<TK, iterator>::value,
                          size_type>::type
  UnsafeErase(const TK& key) {
    return InternalEraseByKey(key);
  }

  node_type UnsafeExtract(const_iterator pos) {
    InternalExtract(pos.NodePtr());
    return ws::concurrency::detail::NodeHandleAccessor::Construct<node_type>(
        pos.NodePtr());
  }

  node_type UnsafeExtract(iterator pos) {
    InternalExtract(pos.NodePtr());
    return ws::concurrency::detail::NodeHandleAccessor::Construct<node_type>(
        pos.NodePtr());
  }

  node_type UnsafeExtract(const key_type& key) {
    iterator item = Find(key);
    return item == end() ? node_type() : UnsafeExtract(item);
  }

  template <typename TK>
  typename std::enable_if<is_transparent<TK>::value &&
                              !std::is_convertible<TK, const_iterator>::value &&
                              !std::is_convertible<TK, iterator>::value,
                          node_type>::type
  UnsafeExtract(const TK& key) {
    iterator item = Find(key);
    return item == end() ? node_type() : UnsafeExtract(item);
  }

  iterator Find(const key_type& key) {
    value_node_ptr result = InternalFind(key);
    return result == nullptr ? end() : iterator(result);
  }

  const_iterator Find(const key_type& key) const {
    value_node_ptr result = const_cast<self_type*>(this)->InternalFind(key);
    return result == nullptr ? end() : const_iterator(result);
  }

  template <typename TK>
  typename std::enable_if<is_transparent<TK>::value, iterator>::type Find(
      const TK& key) {
    value_node_ptr result = InternalFind(key);
    return result == nullptr ? end() : iterator(result);
  }

  template <typename TK>
  typename std::enable_if<is_transparent<TK>::value, const_iterator>::type Find(
      const TK& key) const {
    value_node_ptr result = const_cast<self_type*>(this)->InternalFind(key);
    return result == nullptr ? end() : const_iterator(result);
  }

  std::pair<iterator, iterator> EqualRange(const key_type& key) {
    auto result = InternalEqualRange(key);
    return std::make_pair(iterator(result.first), iterator(result.second));
  }

  std::pair<const_iterator, const_iterator> EqualRange(
      const key_type& key) const {
    auto result = const_cast<self_type*>(this)->InternalEqualRange(key);
    return std::make_pair(const_iterator(result.first),
                          const_iterator(result.second));
  }

  template <typename TK>
  typename std::enable_if<is_transparent<TK>::value,
                          std::pair<iterator, iterator>>::type
  EqualRange(const TK& key) {
    auto result = InternalEqualRange(key);
    return std::make_pair(iterator(result.first), iterator(result.second));
  }

  template <typename TK>
  typename std::enable_if<is_transparent<TK>::value,
                          std::pair<const_iterator, const_iterator>>::type
  EqualRange(const TK& key) const {
    auto result = const_cast<self_type*>(this)->InternalEqualRange(key);
    return std::make_pair(iterator(result.first), iterator(result.second));
  }

  size_type Count(const key_type& key) const { return InternalCount(key); }

  template <typename TK>
  typename std::enable_if<is_transparent<TK>::value, size_type>::type Count(
      const TK& key) const {
    return InternalCount(key);
  }

  bool Contains(const key_type& key) const { return Find(key) != end(); }

  template <typename TK>
  typename std::enable_if<is_transparent<TK>::value, bool>::type Contains(
      const TK& key) const {
    return Find(key) != end();
  }

  local_iterator UnsafeBegin(size_type n) {
    return local_iterator(FirstValueNode(GetBucket(n)));
  }

  const_local_iterator UnsafeBegin(size_type n) const {
    auto bucket_begin =
        FirstValueNode(const_cast<self_type*>(this)->GetBucket(n));
    return const_local_iterator(bucket_begin);
  }

  const_local_iterator UnsafeCBegin(size_type n) const {
    auto bucket_begin =
        FirstValueNode(const_cast<self_type*>(this)->GetBucket(n));
    return const_local_iterator(bucket_begin);
  }

  local_iterator UnsafeEnd(size_type n) {
    size_type bucket_count = bucket_count_.load(std::memory_order_relaxed);
    return n != bucket_count - 1 ? UnsafeBegin(GetNextBucketIndex(n))
                                 : local_iterator(nullptr);
  }

  const_local_iterator UnsafeEnd(size_type n) const {
    size_type bucket_count = bucket_count_.load(std::memory_order_relaxed);
    return n != bucket_count - 1 ? UnsafeBegin(GetNextBucketIndex(n))
                                 : const_local_iterator(nullptr);
  }

  const_local_iterator UnsafeCEnd(size_type n) const {
    size_type bucket_count = bucket_count_.load(std::memory_order_relaxed);
    return n != bucket_count - 1 ? UnsafeBegin(GetNextBucketIndex(n))
                                 : const_local_iterator(nullptr);
  }

  size_type UnsafeBucketCount() const {
    return bucket_count_.load(std::memory_order_relaxed);
  }

  size_type UnsafeMaxBucketCount() const { return MaxSize(); }

  size_type UnsafeBucketSize(size_type n) const {
    return size_type(std::distance(UnsafeBegin(n), UnsafeEnd(n)));
  }

  size_type UnsafeBucket(const key_type& key) const {
    return hash_compare_(key) % bucket_count_.load(std::memory_order_relaxed);
  }

  float LoadFactor() const {
    return float(Size() / float(bucket_count_.load(std::memory_order_acquire)));
  }

  float MaxLoadFactor() const { return max_load_factor_; }

  void MaxLoadFactor(float mlf) {
    if (mlf != mlf || mlf < 0)
      WsException::InvalidArgument("Invalid max load factor").Throw();
    max_load_factor_ = mlf;
  }

  void ReHash(size_type bucket_count) {
    size_type current_bucket_count =
        bucket_count_.load(std::memory_order_acquire);
    if (current_bucket_count < bucket_count) {
      bucket_count_.compare_exchange_strong(current_bucket_count,
                                            RoundUpToPowerOfTwo(bucket_count));
    }
  }

  void Reserve(size_type elements_count) {
    size_type current_bucket_count =
        bucket_count_.load(std::memory_order_acquire);
    size_type necessary_bucket_count = current_bucket_count;

    while (necessary_bucket_count * MaxLoadFactor() < elements_count) {
      necessary_bucket_count <<= 1;
    }

    while (!bucket_count_.compare_exchange_strong(current_bucket_count,
                                                  necessary_bucket_count)) {
      if (current_bucket_count >= necessary_bucket_count) break;
    }
  }

  hasher HashFunction() const { return hash_compare_.HashFunction(); }

  key_equal KeyEq() const { return hash_compare_.KeyEq(); }

  class ConstRangeType {
   private:
    const ConcurrentUnorderedBase& instance_;
    node_ptr begin_node_;
    node_ptr end_node_;
    mutable node_ptr midpoint_node_;

   public:
    using size_type = typename ConcurrentUnorderedBase::size_type;
    using value_type = typename ConcurrentUnorderedBase::value_type;
    using reference = typename ConcurrentUnorderedBase::reference;
    using difference_type = typename ConcurrentUnorderedBase::difference_type;
    using iterator = typename ConcurrentUnorderedBase::const_iterator;

    bool Empty() const { return begin_node_ == end_node_; }

    bool IsDivisible() const { return midpoint_node_ != end_node_; }

    size_type Grainsize() const { return 1; }

    ConstRangeType(ConstRangeType& range, ws::concurrency::detail::range::split)
        : instance_(range.instance_),
          begin_node_(range.midpoint_node_),
          end_node_(range.end_node_) {
      range.end_node_ = begin_node_;
      assert(!Empty() && "Splitting despite the range is not divisible");
      assert(!range.Empty() && "Splitting despite the range is not divisible");
      SetMidpoint();
      range.SetMidpoint();
    }

    iterator begin() const {
      return iterator(instance_.FirstValueNode(begin_node_));
    }
    iterator end() const {
      return iterator(instance_.FirstValueNode(end_node_));
    }

    ConstRangeType(const ConcurrentUnorderedBase& table)
        : instance_(table),
          begin_node_(
              instance_.FirstValueNode(const_cast<node_ptr>(&table.head_))),
          end_node_(nullptr) {
      SetMidpoint();
    }

   private:
    void SetMidpoint() const {
      if (Empty()) {
        midpoint_node_ = end_node_;
      } else {
        sokey_type invalid_key = ~sokey_type(0);
        sokey_type begin_key =
            begin_node_ != nullptr ? begin_node_->OrderKey() : invalid_key;
        sokey_type end_key =
            end_node_ != nullptr ? end_node_->OrderKey() : invalid_key;

        size_type mid_bucket =
            ws::concurrency::detail::ReverseBits(begin_key +
                                                 (end_key - begin_key) / 2) %
            instance_.bucket_count_.load(std::memory_order_relaxed);
        while (instance_.segments_[mid_bucket].load(
                   std::memory_order_relaxed) == nullptr) {
          mid_bucket = instance_.GetParent(mid_bucket);
        }
        if (ws::concurrency::detail::ReverseBits(mid_bucket) > begin_key) {
          midpoint_node_ = instance_.FirstValueNode(
              instance_.segments_[mid_bucket].load(std::memory_order_relaxed));
        } else {
          midpoint_node_ = end_node_;
        }
      }
    }
  };

  class RangeType : public ConstRangeType {
   public:
    using iterator = typename ConcurrentUnorderedBase::iterator;
    using ConstRangeType::ConstRangeType;

    iterator begin() const {
      return iterator(ConstRangeType::begin().NodePtr());
    }
    iterator end() const { return iterator(ConstRangeType::end().NodePtr()); }
  };

  RangeType Range() { return RangeType(*this); }

  ConstRangeType Range() const { return ConstRangeType(*this); }

 protected:
  static constexpr bool kAllowMultimapping = traits_type::kAllowMultimapping;

 private:
  static constexpr size_type kInitialBucketCount = 8;
  static constexpr float kinitialMaxLoadFactor = 4;
  static constexpr size_type kPointersPerEmbeddedTable =
      sizeof(size_type) * 8 - 1;

  class UnorderedSegmentTable
      : public ws::concurrency::detail::SegmentTable<
            std::atomic<node_ptr>, allocator_type, UnorderedSegmentTable,
            kPointersPerEmbeddedTable> {
    using self_type = UnorderedSegmentTable;
    using atomic_node_ptr = std::atomic<node_ptr>;
    using base_type = ws::concurrency::detail::SegmentTable<
        std::atomic<node_ptr>, allocator_type, UnorderedSegmentTable,
        kPointersPerEmbeddedTable>;
    using segment_type = typename base_type::segment_type;
    using base_allocator_type = typename base_type::allocator_type;

    using segment_allocator_type =
        typename allocator_traits_type::template rebind_alloc<atomic_node_ptr>;
    using segment_allocator_traits =
        std::allocator_traits<segment_allocator_type>;

   public:
    static constexpr bool kAllowTableExtending = false;
    static constexpr bool kIsNoExceptAssignment =
        std::is_nothrow_move_assignable<hasher>::value &&
        std::is_nothrow_move_assignable<key_equal>::value &&
        segment_allocator_traits::is_always_equal::value;
    static constexpr bool kIsNoExceptSwap =
        std::is_nothrow_swappable<hasher>::value &&
        std::is_nothrow_swappable<key_equal>::value &&
        segment_allocator_traits::is_always_equal::value;

    UnorderedSegmentTable(
        const base_allocator_type& alloc = base_allocator_type())
        : base_type(alloc) {}

    UnorderedSegmentTable(const UnorderedSegmentTable&) = default;

    UnorderedSegmentTable(const UnorderedSegmentTable& other,
                          const base_allocator_type& alloc)
        : base_type(other, alloc) {}

    UnorderedSegmentTable(UnorderedSegmentTable&&) = default;

    UnorderedSegmentTable(UnorderedSegmentTable&& other,
                          const base_allocator_type& alloc)
        : base_type(std::move(other), alloc) {}

    UnorderedSegmentTable& operator=(const UnorderedSegmentTable&) = default;

    UnorderedSegmentTable& operator=(UnorderedSegmentTable&&) = default;

    segment_type CreateSegment(
        typename base_type::segment_table_type,
        typename base_type::segment_index_type segment_index, size_type) {
      segment_allocator_type alloc(this->get_allocator());
      size_type seg_size = this->SegmentSize(segment_index);
      segment_type new_segment =
          segment_allocator_traits::allocate(alloc, seg_size);
      for (size_type i = 0; i != seg_size; ++i) {
        segment_allocator_traits::construct(alloc, new_segment + i, nullptr);
      }
      return new_segment;
    }

    segment_type NullifySegment(typename base_type::segment_table_type table,
                                size_type segment_index) {
      segment_type target_segment =
          table[segment_index].load(std::memory_order_relaxed);
      table[segment_index].store(nullptr, std::memory_order_relaxed);
      return target_segment;
    }

    void DeallocateSegment(segment_type address, size_type index) {
      DestroySegment(address, index);
    }

    void DestroySegment(segment_type address, size_type index) {
      segment_allocator_type alloc(this->get_allocator());
      for (size_type i = 0; i != this->SegmentSize(index); ++i) {
        segment_allocator_traits::destroy(alloc, address + i);
      }
      segment_allocator_traits::deallocate(alloc, address,
                                           this->SegmentSize(index));
    }

    void CopySegment(size_type index, segment_type, segment_type to) {
      if (index == 0) {
        to[1].store(nullptr, std::memory_order_relaxed);
      } else {
        for (size_type i = 0; i != this->SegmentSize(index); ++i) {
          to[i].store(nullptr, std::memory_order_relaxed);
        }
      }
    }

    void MoveSegment(size_type index, segment_type from, segment_type to) {
      if (index == 0) {
        to[1].store(from[1].load(std::memory_order_relaxed),
                    std::memory_order_relaxed);
      } else {
        for (size_type i = 0; i != this->SegmentSize(index); ++i) {
          to[i].store(from[i].load(std::memory_order_relaxed),
                      std::memory_order_relaxed);
          from[i].store(nullptr, std::memory_order_relaxed);
        }
      }
    }

    typename base_type::segment_table_type AllocateLongTable(
        const typename base_type::atomic_segment*, size_type) {
      assert(false && "This method should never been called");

      return nullptr;
    }

    void DestroyElements() {}
  };

  void InternalClear() {
    node_ptr next = head_.Next();
    node_ptr curr = next;

    head_.SetNext(nullptr);

    while (curr != nullptr) {
      next = curr->Next();
      DestroyNode(curr);
      curr = next;
    }

    size_.store(0, std::memory_order_relaxed);
    segments_.Clear();
  }

  void DestroyNode(node_ptr node) {
    if (node->IsDummy()) {
      node_allocator_type dummy_node_allocator(segments_.get_allocator());

      node_allocator_traits::destroy(dummy_node_allocator, node);

      node_allocator_traits::deallocate(dummy_node_allocator, node, 1);
    } else {
      value_node_ptr val_node = static_cast<value_node_ptr>(node);
      value_node_allocator_type value_node_allocator(segments_.get_allocator());

      value_node_allocator_traits::destroy(value_node_allocator,
                                           val_node->Storage());

      value_node_allocator_traits::destroy(value_node_allocator, val_node);

      value_node_allocator_traits::deallocate(value_node_allocator, val_node,
                                              1);
    }
  }

  struct InternalInsertReturnType {
    value_node_ptr remaining_node;

    value_node_ptr node_with_equal_key;

    bool inserted;
  };

  template <typename TValueType>
  std::pair<iterator, bool> InternalInsertValue(TValueType&& value) {
    auto create_value_node = [&value,
                              this](sokey_type order_key) -> value_node_ptr {
      return CreateNode(order_key, std::forward<TValueType>(value));
    };

    auto insert_result = InternalInsert(value, create_value_node);

    if (insert_result.remaining_node != nullptr) {
      assert(!insert_result.inserted &&
             "remaining_node should be nullptr if the node was "
             "successfully inserted");
      DestroyNode(insert_result.remaining_node);
    }

    return {iterator(insert_result.node_with_equal_key),
            insert_result.inserted};
  }

  template <typename TValueType, typename TCreateInsertNode>
  InternalInsertReturnType InternalInsert(
      TValueType&& value, TCreateInsertNode create_insert_node) {
    static_assert(
        std::is_same<typename std::decay<TValueType>::type, value_type>::value,
        "Incorrect type in internal_insert");
    const key_type& key = traits_type::GetKey(value);
    sokey_type hash_key = sokey_type(hash_compare_(key));

    sokey_type order_key = SplitOrderKeyRegular(hash_key);
    node_ptr prev = PrepareBucket(hash_key);
    assert(prev != nullptr && "Invalid head node");

    auto search_result = SearchAfter(prev, order_key, key);

    if (search_result.second) {
      return InternalInsertReturnType{nullptr, search_result.first, false};
    }

    value_node_ptr new_node = create_insert_node(order_key);
    node_ptr curr = search_result.first;

    while (!TryInsert(prev, new_node, curr)) {
      search_result = SearchAfter(prev, order_key, key);
      if (search_result.second) {
        return InternalInsertReturnType{new_node, search_result.first, false};
      }
      curr = search_result.first;
    }

    auto sz = size_.fetch_add(1);
    AdjustTableSize(sz + 1, bucket_count_.load(std::memory_order_acquire));
    return InternalInsertReturnType{
        nullptr, static_cast<value_node_ptr>(new_node), true};
  }

  std::pair<value_node_ptr, bool> SearchAfter(node_ptr& prev,
                                              sokey_type order_key,
                                              const key_type& key) {
    node_ptr curr = prev->Next();

    while (curr != nullptr &&
           (curr->OrderKey() < order_key ||
            (curr->OrderKey() == order_key &&
             !hash_compare_(traits_type::GetKey(
                                static_cast<value_node_ptr>(curr)->Value()),
                            key)))) {
      prev = curr;
      curr = curr->Next();
    }

    if (curr != nullptr && curr->OrderKey() == order_key &&
        !kAllowMultimapping) {
      return {static_cast<value_node_ptr>(curr), true};
    }
    return {static_cast<value_node_ptr>(curr), false};
  }

  void AdjustTableSize(size_type total_elements, size_type current_size) {
    if ((float(total_elements) / float(current_size)) > max_load_factor_) {
      bucket_count_.compare_exchange_strong(current_size, 2u * current_size);
    }
  }

  node_ptr InsertDummyNode(node_ptr parent_dummy_node, sokey_type order_key) {
    node_ptr prev_node = parent_dummy_node;

    node_ptr dummy_node = CreateDummyNode(order_key);
    node_ptr next_node;

    do {
      next_node = prev_node->Next();

      while (next_node != nullptr && next_node->OrderKey() < order_key) {
        prev_node = next_node;
        next_node = next_node->Next();
      }

      if (next_node != nullptr && next_node->OrderKey() == order_key) {
        DestroyNode(dummy_node);
        return next_node;
      }
    } while (!TryInsert(prev_node, dummy_node, next_node));

    return dummy_node;
  }

  static bool TryInsert(node_ptr prev_node, node_ptr new_node,
                        node_ptr current_next_node) {
    new_node->SetNext(current_next_node);
    return prev_node->TrySetNext(current_next_node, new_node);
  }

  node_ptr PrepareBucket(sokey_type hash_key) {
    size_type bucket = hash_key % bucket_count_.load(std::memory_order_acquire);
    return GetBucket(bucket);
  }

  node_ptr GetBucket(size_type bucket_index) {
    if (segments_[bucket_index].load(std::memory_order_acquire) == nullptr) {
      InitBucket(bucket_index);
    }
    return segments_[bucket_index].load(std::memory_order_acquire);
  }

  void InitBucket(size_type bucket) {
    if (bucket == 0) {
      node_ptr disabled = nullptr;
      segments_[0].compare_exchange_strong(disabled, &head_);
      return;
    }

    size_type parent_bucket = GetParent(bucket);

    while (segments_[parent_bucket].load(std::memory_order_acquire) ==
           nullptr) {
      InitBucket(parent_bucket);
    }

    assert(segments_[parent_bucket].load(std::memory_order_acquire) !=
               nullptr &&
           "Parent bucket should be initialized");
    node_ptr parent = segments_[parent_bucket].load(std::memory_order_acquire);

    node_ptr dummy_node = InsertDummyNode(parent, SplitOrderKeyDummy(bucket));

    segments_[bucket].store(dummy_node, std::memory_order_release);
  }

  node_ptr CreateDummyNode(sokey_type order_key) {
    node_allocator_type dummy_node_allocator(segments_.get_allocator());
    node_ptr dummy_node =
        node_allocator_traits::allocate(dummy_node_allocator, 1);
    node_allocator_traits::construct(dummy_node_allocator, dummy_node,
                                     order_key);
    return dummy_node;
  }

  template <typename... TArgs>
  value_node_ptr CreateNode(sokey_type order_key, TArgs&&... args) {
    value_node_allocator_type value_node_allocator(segments_.get_allocator());

    value_node_ptr new_node =
        value_node_allocator_traits::allocate(value_node_allocator, 1);

    value_node_allocator_traits::construct(value_node_allocator, new_node,
                                           order_key);

    auto value_guard = ws::concurrency::detail::templates::MakeRaiiGuard([&] {
      value_node_allocator_traits::destroy(value_node_allocator, new_node);
      value_node_allocator_traits::deallocate(value_node_allocator, new_node,
                                              1);
    });

    value_node_allocator_traits::construct(value_node_allocator,
                                           new_node->Storage(),
                                           std::forward<TArgs>(args)...);
    value_guard.Dismiss();
    return new_node;
  }

  value_node_ptr FirstValueNode(node_ptr first_node) const {
    while (first_node != nullptr && first_node->IsDummy()) {
      first_node = first_node->Next();
    }
    return static_cast<value_node_ptr>(first_node);
  }

  node_ptr InternalErase(value_node_ptr node_to_erase) {
    assert(node_to_erase != nullptr && "Invalid iterator for erase");
    node_ptr next_node = node_to_erase->Next();
    InternalExtract(node_to_erase);
    DestroyNode(node_to_erase);
    return next_node;
  }

  template <typename TK>
  size_type InternalEraseByKey(const TK& key) {
    auto eq_range = EqualRange(key);
    size_type erased_count = 0;

    for (auto it = eq_range.first; it != eq_range.second;) {
      it = UnsafeErase(it);
      ++erased_count;
    }
    return erased_count;
  }

  void InternalExtract(value_node_ptr node_to_extract) {
    const key_type& key = traits_type::GetKey(node_to_extract->Value());
    sokey_type hash_key = sokey_type(hash_compare_(key));

    node_ptr prev_node = PrepareBucket(hash_key);

    for (node_ptr node = prev_node->Next(); node != nullptr;
         prev_node = node, node = node->Next()) {
      if (node == node_to_extract) {
        UnlinkNode(prev_node, node, node_to_extract->Next());
        size_.store(size_.load(std::memory_order_relaxed) - 1,
                    std::memory_order_relaxed);
        return;
      }
      assert(node->OrderKey() <= node_to_extract->OrderKey() &&
             "node, which is going to be extracted should be presented "
             "in the list");
    }
  }

 protected:
  template <typename TSourceType>
  void InternalMerge(TSourceType&& source) {
    static_assert(
        std::is_same<node_type,
                     typename std::decay<TSourceType>::type::node_type>::value,
        "Incompatible containers cannot be merged");

    for (node_ptr source_prev = &source.head_;
         source_prev->Next() != nullptr;) {
      if (!source_prev->Next()->IsDummy()) {
        value_node_ptr curr = static_cast<value_node_ptr>(source_prev->Next());

        if (kAllowMultimapping ||
            !Contains(traits_type::GetKey(curr->Value()))) {
          node_ptr next_node = curr->Next();
          source.UnlinkNode(source_prev, curr, next_node);

          sokey_type old_order_key = curr->OrderKey();

          node_type curr_node =
              ws::concurrency::detail::NodeHandleAccessor::Construct<node_type>(
                  curr);

          if (!Insert(std::move(curr_node)).second) {
            assert(!kAllowMultimapping &&
                   "Insertion should succeed for multicontainer");
            assert(source_prev->Next() == next_node &&
                   "Concurrent operations with the source container in "
                   "merge are prohibited");

            curr->Init(old_order_key);
            assert(old_order_key >= source_prev->OrderKey() &&
                   (next_node == nullptr ||
                    old_order_key <= next_node->OrderKey()) &&
                   "Wrong nodes order in the source container");

            curr->SetNext(next_node);
            source_prev->SetNext(curr);
            source_prev = curr;
            ws::concurrency::detail::NodeHandleAccessor::Deactivate(curr_node);
          } else {
            source.size_.fetch_sub(1, std::memory_order_relaxed);
          }
        } else {
          source_prev = curr;
        }
      } else {
        source_prev = source_prev->Next();
      }
    }
  }

 private:
  void UnlinkNode(node_ptr prev_node, node_ptr node_to_unlink,
                  node_ptr next_node) {
    assert((prev_node->Next() == node_to_unlink &&
            node_to_unlink->Next() == next_node) &&
           "erasing and extracting nodes from the containers are unsafe "
           "in concurrent mode");
    prev_node->SetNext(next_node);
    node_to_unlink->SetNext(nullptr);
  }

  template <typename TK>
  value_node_ptr InternalFind(const TK& key) {
    sokey_type hash_key = sokey_type(hash_compare_(key));
    sokey_type order_key = SplitOrderKeyRegular(hash_key);

    node_ptr curr = PrepareBucket(hash_key);

    while (curr != nullptr) {
      if (curr->OrderKey() > order_key) {
        return nullptr;
      } else if (curr->OrderKey() == order_key &&
                 hash_compare_(traits_type::GetKey(
                                   static_cast<value_node_ptr>(curr)->Value()),
                               key)) {
        return static_cast<value_node_ptr>(curr);
      }
      curr = curr->Next();
    }

    return nullptr;
  }

  template <typename TK>
  std::pair<value_node_ptr, value_node_ptr> InternalEqualRange(const TK& key) {
    sokey_type hash_key = sokey_type(hash_compare_(key));
    sokey_type order_key = SplitOrderKeyRegular(hash_key);

    node_ptr curr = PrepareBucket(hash_key);

    while (curr != nullptr) {
      if (curr->OrderKey() > order_key) {
        return std::make_pair(nullptr, nullptr);
      } else if (curr->OrderKey() == order_key &&
                 hash_compare_(traits_type::GetKey(
                                   static_cast<value_node_ptr>(curr)->Value()),
                               key)) {
        value_node_ptr first = static_cast<value_node_ptr>(curr);
        node_ptr last = first;
        do {
          last = last->Next();
        } while (kAllowMultimapping && last != nullptr && !last->IsDummy() &&
                 hash_compare_(traits_type::GetKey(
                                   static_cast<value_node_ptr>(last)->Value()),
                               key));
        return std::make_pair(first, FirstValueNode(last));
      }
      curr = curr->Next();
    }
    return {nullptr, nullptr};
  }

  template <typename TK>
  size_type InternalCount(const TK& key) const {
    if (kAllowMultimapping) {
      auto eq_range = EqualRange(key);
      return std::distance(eq_range.first, eq_range.second);
    } else {
      return Contains(key) ? 1 : 0;
    }
  }

  void InternalCopy(const ConcurrentUnorderedBase& other) {
    node_ptr last_node = &head_;
    segments_[0].store(&head_, std::memory_order_relaxed);

    for (node_ptr node = other.head_.Next(); node != nullptr;
         node = node->Next()) {
      node_ptr new_node;
      if (!node->IsDummy()) {
        new_node = CreateNode(node->OrderKey(),
                              static_cast<value_node_ptr>(node)->Value());
      } else {
        new_node = CreateDummyNode(node->OrderKey());
        segments_[ws::concurrency::detail::ReverseBits(node->OrderKey())].store(
            new_node, std::memory_order_relaxed);
      }

      last_node->SetNext(new_node);
      last_node = new_node;
    }
  }

  void InternalMove(ConcurrentUnorderedBase&& other) {
    node_ptr last_node = &head_;
    segments_[0].store(&head_, std::memory_order_relaxed);

    for (node_ptr node = other.head_.Next(); node != nullptr;
         node = node->Next()) {
      node_ptr new_node;
      if (!node->IsDummy()) {
        new_node =
            CreateNode(node->OrderKey(),
                       std::move(static_cast<value_node_ptr>(node)->value()));
      } else {
        new_node = CreateDummyNode(node->OrderKey());
        segments_[ws::concurrency::detail::ReverseBits(node->OrderKey())].store(
            new_node, std::memory_order_relaxed);
      }

      last_node->SetNext(new_node);
      last_node = new_node;
    }
  }

  void MoveContent(ConcurrentUnorderedBase&& other) {
    head_.SetNext(other.head_.Next());
    other.head_.SetNext(nullptr);
    segments_[0].store(&head_, std::memory_order_relaxed);

    other.bucket_count_.store(kInitialBucketCount, std::memory_order_relaxed);
    other.max_load_factor_ = kinitialMaxLoadFactor;
    other.size_.store(0, std::memory_order_relaxed);
  }

  void InternalMoveConstructWithAllocator(
      ConcurrentUnorderedBase&& other, const allocator_type&,
      /*is_always_equal = */ std::true_type) {
    MoveContent(std::move(other));
  }

  void InternalMoveConstructWithAllocator(
      ConcurrentUnorderedBase&& other, const allocator_type& alloc,
      /*is_always_equal = */ std::false_type) {
    if (alloc == other.segments_.get_allocator()) {
      MoveContent(std::move(other));
    } else {
      TryCall([&] { InternalMove(std::move(other)); }).OnException([&] {
        Clear();
      });
    }
  }

  void InternalMoveAssign(ConcurrentUnorderedBase&& other,
                          /*is_always_equal || POCMA = */ std::true_type) {
    MoveContent(std::move(other));
  }

  void InternalMoveAssign(ConcurrentUnorderedBase&& other,
                          /*is_always_equal || POCMA = */ std::false_type) {
    if (segments_.get_allocator() == other.segments_.get_allocator()) {
      MoveContent(std::move(other));
    } else {
      InternalMove(std::move(other));
    }
  }

  void InternalSwap(ConcurrentUnorderedBase& other,
                    /*is_always_equal || POCS = */ std::true_type) {
    InternalSwapFields(other);
  }

  void InternalSwap(ConcurrentUnorderedBase& other,
                    /*is_always_equal || POCS = */ std::false_type) {
    assert(segments_.get_allocator() == other.segments_.get_allocator() &&
           "Swapping with unequal allocators is not allowed");
    InternalSwapFields(other);
  }

  void InternalSwapFields(ConcurrentUnorderedBase& other) {
    node_ptr first_node = head_.Next();
    head_.SetNext(other.head_.Next());
    other.head_.SetNext(first_node);

    size_type current_size = size_.load(std::memory_order_relaxed);
    size_.store(other.size_.load(std::memory_order_relaxed),
                std::memory_order_relaxed);
    other.size_.store(current_size, std::memory_order_relaxed);

    size_type bucket_count = bucket_count_.load(std::memory_order_relaxed);
    bucket_count_.store(other.bucket_count_.load(std::memory_order_relaxed),
                        std::memory_order_relaxed);
    other.bucket_count_.store(bucket_count, std::memory_order_relaxed);

    using std::swap;
    swap(max_load_factor_, other.max_load_factor_);
    swap(hash_compare_, other.hash_compare_);
    segments_.Swap(other.segments_);

    segments_[0].store(&head_, std::memory_order_relaxed);
    other.segments_[0].store(&other.head_, std::memory_order_relaxed);
  }

  static constexpr sokey_type SplitOrderKeyRegular(sokey_type hash) {
    return ws::concurrency::detail::ReverseBits(hash) | 0x1;
  }

  static constexpr sokey_type SplitOrderKeyDummy(sokey_type hash) {
    return ws::concurrency::detail::ReverseBits(hash) & ~sokey_type(0x1);
  }

  size_type GetParent(size_type bucket) const {
    assert(bucket != 0 && "Unable to GetParent of the bucket 0");
    size_type msb = ws::concurrency::detail::Log2(bucket);
    return bucket & ~(size_type(1) << msb);
  }

  size_type GetNextBucketIndex(size_type bucket) const {
    size_type bits = ws::concurrency::detail::Log2(
        bucket_count_.load(std::memory_order_relaxed));
    size_type reversed_next =
        ws::concurrency::detail::ReverseNBits(bucket, bits) + 1;
    return ws::concurrency::detail::ReverseNBits(reversed_next, bits);
  }

  std::atomic<size_type> size_;
  std::atomic<size_type> bucket_count_;
  float max_load_factor_;
  hash_compare_type hash_compare_;

  list_node_type head_;
  UnorderedSegmentTable segments_;

  template <typename TContainer, typename Value>
  friend class SolistIterator;

  template <typename TOtherTraits>
  friend class ConcurrentUnorderedBase;
};

template <typename TTraits>
bool operator==(const ConcurrentUnorderedBase<TTraits>& lhs,
                const ConcurrentUnorderedBase<TTraits>& rhs) {
  if (&lhs == &rhs) {
    return true;
  }

  if (lhs.Size() != rhs.Size()) {
    return false;
  }

#if _MSC_VER
  return std::is_permutation(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
#else
  return std::is_permutation(lhs.begin(), lhs.end(), rhs.begin());
#endif
}

#if !_CPP20_COMPARISONS_PRESENT
template <typename Traits>
bool operator!=(const concurrent_unordered_base<Traits>& lhs,
                const concurrent_unordered_base<Traits>& rhs) {
  return !(lhs == rhs);
}
#endif

#if defined(_MSC_VER) && !defined(__INTEL_COMPILER)
#pragma warning(pop)
#endif

}  // namespace detail
}  // namespace concurrency
}  // namespace ws
