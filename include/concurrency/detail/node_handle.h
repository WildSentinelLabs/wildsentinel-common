#pragma once

#include "arch/config.h"
#include "concurrency/detail/allocator_traits.h"

namespace ws {
namespace concurrency {
namespace detail {

struct NodeHandleAccessor {
  template <typename TNodeHandleType>
  static typename TNodeHandleType::node* GetNodePtr(TNodeHandleType& nh) {
    return nh.NodePtr();
  }

  template <typename TNodeHandleType>
  static TNodeHandleType Construct(typename TNodeHandleType::node* node_ptr) {
    return TNodeHandleType{node_ptr};
  }

  template <typename TNodeHandleType>
  static void Deactivate(TNodeHandleType& nh) {
    nh.Deactivate();
  }
};

template <typename TValue, typename TNode, typename TAllocator>
class NodeHandleBase {
 public:
  using allocator_type = TAllocator;

 protected:
  using node = TNode;
  using allocator_traits_type = std::allocator_traits<allocator_type>;

 public:
  NodeHandleBase() : node_(nullptr), allocator_() {}
  NodeHandleBase(NodeHandleBase&& nh)
      : node_(nh.node_), allocator_(std::move(nh.allocator_)) {
    nh.node_ = nullptr;
  }

  _GLIBCXX_NODISCARD bool Empty() const { return node_ == nullptr; }
  explicit operator bool() const { return node_ != nullptr; }

  ~NodeHandleBase() { InternalDestroy(); }

  NodeHandleBase& operator=(NodeHandleBase&& nh) {
    InternalDestroy();
    node_ = nh.node_;
    MoveAssignAllocators(allocator_, nh.allocator_);
    nh.Deactivate();
    return *this;
  }

  void Swap(NodeHandleBase& nh) {
    using std::swap;
    swap(node_, nh.node_);
    SwapAllocators(allocator_, nh.allocator_);
  }

  allocator_type get_allocator() const { return allocator_; }

 protected:
  NodeHandleBase(node* n) : node_(n) {}

  void InternalDestroy() {
    if (node_ != nullptr) {
      allocator_traits_type::destroy(allocator_, node_->Storage());
      typename allocator_traits_type::template rebind_alloc<node>
          node_allocator(allocator_);
      node_allocator.deallocate(node_, 1);
    }
  }

  node* NodePtr() { return node_; }

  void Deactivate() { node_ = nullptr; }

  node* node_;
  allocator_type allocator_;
};

template <typename TKey, typename TValue, typename TNode, typename TAllocator>
class NodeHandle : public NodeHandleBase<TValue, TNode, TAllocator> {
  using base_type = NodeHandleBase<TValue, TNode, TAllocator>;

 public:
  using key_type = TKey;
  using mapped_type = typename TValue::second_type;
  using allocator_type = typename base_type::allocator_type;

  NodeHandle() = default;

  key_type& Key() const {
    assert(!this->Empty() && "Cannot get key from the empty node_type object");
    return *const_cast<key_type*>(&(this->node_->Value().first));
  }

  mapped_type& Mapped() const {
    assert(!this->Empty() &&
           "Cannot get mapped value from the empty node_type object");
    return this->node_->Value().second;
  }

 private:
  friend struct NodeHandleAccessor;

  NodeHandle(typename base_type::node* n) : base_type(n) {}
};

template <typename TKey, typename TNode, typename TAllocator>
class NodeHandle<TKey, TKey, TNode, TAllocator>
    : public NodeHandleBase<TKey, TNode, TAllocator> {
  using base_type = NodeHandleBase<TKey, TNode, TAllocator>;

 public:
  using value_type = TKey;
  using allocator_type = typename base_type::allocator_type;

  NodeHandle() = default;

  value_type& Value() const {
    assert(!this->Empty() &&
           "Cannot get value from the empty node_type object");
    return *const_cast<value_type*>(&(this->node_->Value()));
  }

 private:
  friend struct NodeHandleAccessor;

  NodeHandle(typename base_type::node* n) : base_type(n) {}
};

template <typename TKey, typename TValue, typename TNode, typename TAllocator>
void Swap(NodeHandle<TKey, TValue, TNode, TAllocator>& lhs,
          NodeHandle<TKey, TValue, TNode, TAllocator>& rhs) {
  return lhs.Swap(rhs);
}

}  // namespace detail
}  // namespace concurrency
}  // namespace ws
