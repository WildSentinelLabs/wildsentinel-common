#pragma once

#include <atomic>
#include <semaphore>

#include "arch/cpu_arch.h"
#include "concurrency/detail/aligned_space.h"
#include "concurrency/detail/helpers.h"
#include "concurrency/spin_mutex.h"
#include "concurrent_monitor_mutex.h"

namespace ws {
namespace concurrency {
namespace detail {

inline static void AtomicFenceSeqCst() {
  std::atomic_thread_fence(std::memory_order_seq_cst);
}

class CircularDoublyLinkedListWithSentinel {
 public:
  struct BaseNode {
    BaseNode* next;
    BaseNode* prev;

    constexpr BaseNode(BaseNode* n, BaseNode* p) : next(n), prev(p) {}
    explicit BaseNode()
        : next((BaseNode*)(uintptr_t)0xcdcdcdcd),
          prev((BaseNode*)(uintptr_t)0xcdcdcdcd) {}
  };

  constexpr CircularDoublyLinkedListWithSentinel()
      : count_(0), head_(&head_, &head_) {}

  CircularDoublyLinkedListWithSentinel(
      const CircularDoublyLinkedListWithSentinel&) = delete;
  CircularDoublyLinkedListWithSentinel& operator=(
      const CircularDoublyLinkedListWithSentinel&) = delete;

  inline std::size_t Size() const {
    return count_.load(std::memory_order_relaxed);
  }

  inline bool Empty() const { return Size() == 0; }

  inline BaseNode* Front() const { return head_.next; }

  inline BaseNode* Last() const { return head_.prev; }

  inline const BaseNode* End() const { return &head_; }

  inline void Add(BaseNode* n) {
    count_.store(count_.load(std::memory_order_relaxed) + 1,
                 std::memory_order_relaxed);
    n->prev = head_.prev;
    n->next = &head_;
    head_.prev->next = n;
    head_.prev = n;
  }

  inline void Remove(BaseNode& n) {
    assert(count_.load(std::memory_order_relaxed) > 0 &&
           "attempt to remove an item from an empty list");
    count_.store(count_.load(std::memory_order_relaxed) - 1,
                 std::memory_order_relaxed);
    n.prev->next = n.next;
    n.next->prev = n.prev;
  }

  inline void FlushTo(CircularDoublyLinkedListWithSentinel& lst) {
    const std::size_t l_count = Size();
    if (l_count > 0) {
      lst.count_.store(l_count, std::memory_order_relaxed);
      lst.head_.next = head_.next;
      lst.head_.prev = head_.prev;
      head_.next->prev = &lst.head_;
      head_.prev->next = &lst.head_;
      Clear();
    }
  }

  void Clear() {
    head_.next = &head_;
    head_.prev = &head_;
    count_.store(0, std::memory_order_relaxed);
  }

 private:
  std::atomic<std::size_t> count_;
  BaseNode head_;
};

using base_list = CircularDoublyLinkedListWithSentinel;
using base_node = CircularDoublyLinkedListWithSentinel::BaseNode;

template <typename TContext>
class ConcurrentMonitorBase;

template <typename TContext>
class WaitNode : public base_node {
 public:
  WaitNode(TContext ctx) : context_(ctx) {}

  virtual ~WaitNode() = default;

  virtual void Init() {
    assert(!initialized_);
    initialized_ = true;
  }

  virtual void Wait() = 0;

  virtual void Reset() {
    assert(skipped_wakeup_);
    skipped_wakeup_ = false;
  }

  virtual void Notify() = 0;

 protected:
  friend class ConcurrentMonitorBase<TContext>;
  friend class thread_data;

  TContext context_{};

  std::atomic<bool> is_in_list_{false};

  bool initialized_{false};
  bool skipped_wakeup_{false};
  bool aborted_{false};
  unsigned epoch_{0};
};

template <typename TContext>
class SleepNode : public WaitNode<TContext> {
  using base_type = WaitNode<TContext>;

 public:
  using base_type::base_type;

  ~SleepNode() override {
    if (this->initialized_) {
      if (this->skipped_wakeup_) Semaphore().acquire();
      Semaphore().~counting_semaphore<1>();
    }
  }

  std::binary_semaphore& Semaphore() { return *semaphore_.begin(); }

  void Init() override {
    if (!this->initialized_) {
      new (semaphore_.begin()) std::binary_semaphore(0);
      base_type::Init();
    }
  }

  void Wait() override {
    assert(this->initialized_ &&
           "Use of CommitWait() without prior PrepareWait()");
    Semaphore().acquire();
    assert(!this->is_in_list_.load(std::memory_order_relaxed) &&
           "Still in the queue?");
    if (this->aborted_) throw std::runtime_error("user_abort");
  }

  void Reset() override {
    base_type::Reset();
    Semaphore().acquire();
  }

  void Notify() override { Semaphore().release(); }

 private:
  ws::concurrency::detail::AlignedSpace<std::binary_semaphore> semaphore_;
};

template <typename TContext>
class ConcurrentMonitorBase {
 public:
  constexpr ConcurrentMonitorBase() {}

  ~ConcurrentMonitorBase() = default;

  ConcurrentMonitorBase(const ConcurrentMonitorBase&) = delete;
  ConcurrentMonitorBase& operator=(const ConcurrentMonitorBase&) = delete;

  void PrepareWait(WaitNode<TContext>& node) {
    if (!node.initialized_) {
      node.Init();
    }

    else if (node.skipped_wakeup_) {
      node.Reset();
    }

    node.is_in_list_.store(true, std::memory_order_relaxed);

    {
      ConcurrentMonitorMutex::scoped_lock l(mutex_);
      node.epoch_ = epoch_.load(std::memory_order_relaxed);
      waitset_.Add(&node);
    }

    AtomicFenceSeqCst();
  }

  inline bool CommitWait(WaitNode<TContext>& node) {
    const bool do_it = node.epoch_ == epoch_.load(std::memory_order_relaxed);

    if (do_it) {
      node.Wait();
    } else {
      CancelWait(node);
    }
    return do_it;
  }

  void CancelWait(WaitNode<TContext>& node) {
    node.skipped_wakeup_ = true;

    bool in_list = node.is_in_list_.load(std::memory_order_acquire);
    if (in_list) {
      ConcurrentMonitorMutex::scoped_lock l(mutex_);
      if (node.is_in_list_.load(std::memory_order_relaxed)) {
        waitset_.Remove(node);

        node.is_in_list_.store(false, std::memory_order_relaxed);
        node.skipped_wakeup_ = false;
      }
    }
  }

  template <typename TNodeType, typename TPred>
  bool Wait(TPred&& pred, TNodeType&& node) {
    PrepareWait(node);
    while (!GuardedCall(std::forward<TPred>(pred), node)) {
      if (CommitWait(node)) {
        return true;
      }

      PrepareWait(node);
    }

    CancelWait(node);
    return false;
  }

  void NotifyOne() {
    AtomicFenceSeqCst();
    NotifyOneRelaxed();
  }

  void NotifyOneRelaxed() {
    if (waitset_.Empty()) {
      return;
    }

    base_node* n;
    const base_node* end = waitset_.End();
    {
      ConcurrentMonitorMutex::scoped_lock l(mutex_);
      epoch_.store(epoch_.load(std::memory_order_relaxed) + 1,
                   std::memory_order_relaxed);
      n = waitset_.Front();
      if (n != end) {
        waitset_.Remove(*n);

        ToWaitNode(n)->is_in_list_.store(false, std::memory_order_relaxed);
      }
    }

    if (n != end) {
      ToWaitNode(n)->Notify();
    }
  }

  void NotifyAll() {
    AtomicFenceSeqCst();
    NotifyAllRelaxed();
  }

  void NotifyAllRelaxed() {
    if (waitset_.Empty()) {
      return;
    }

    base_list temp;
    const base_node* end;
    {
      ConcurrentMonitorMutex::scoped_lock l(mutex_);
      epoch_.store(epoch_.load(std::memory_order_relaxed) + 1,
                   std::memory_order_relaxed);

      waitset_.FlushTo(temp);
      end = temp.End();
      for (base_node* n = temp.Front(); n != end; n = n->next) {
        ToWaitNode(n)->is_in_list_.store(false, std::memory_order_relaxed);
      }
    }

    base_node* nxt;
    for (base_node* n = temp.Front(); n != end; n = nxt) {
      nxt = n->next;
      ToWaitNode(n)->Notify();
    }
  }

  template <typename TP>
  void Notify(const TP& predicate) {
    AtomicFenceSeqCst();
    NotifyRelaxed(predicate);
  }

  template <typename TP>
  void NotifyRelaxed(const TP& predicate) {
    if (waitset_.Empty()) {
      return;
    }

    base_list temp;
    base_node* nxt;
    const base_node* end = waitset_.End();
    {
      ConcurrentMonitorMutex::scoped_lock l(mutex_);
      epoch_.store(epoch_.load(std::memory_order_relaxed) + 1,
                   std::memory_order_relaxed);
      for (base_node* n = waitset_.Last(); n != end; n = nxt) {
        nxt = n->prev;
        auto* node = static_cast<WaitNode<TContext>*>(n);
        if (predicate(node->context_)) {
          waitset_.Remove(*n);
          node->is_in_list_.store(false, std::memory_order_relaxed);
          temp.Add(n);
        }
      }
    }

    end = temp.End();
    for (base_node* n = temp.Front(); n != end; n = nxt) {
      nxt = n->next;
      ToWaitNode(n)->Notify();
    }
  }

  template <typename TP>
  void NotifyOneRelaxed(const TP& predicate) {
    if (waitset_.Empty()) {
      return;
    }

    base_node* tmp = nullptr;
    base_node* next{};
    const base_node* end = waitset_.End();
    {
      ConcurrentMonitorMutex::scoped_lock l(mutex_);
      epoch_.store(epoch_.load(std::memory_order_relaxed) + 1,
                   std::memory_order_relaxed);
      for (base_node* n = waitset_.Last(); n != end; n = next) {
        next = n->prev;
        auto* node = static_cast<WaitNode<TContext>*>(n);
        if (predicate(node->context_)) {
          waitset_.Remove(*n);
          node->is_in_list_.store(false, std::memory_order_relaxed);
          tmp = n;
          break;
        }
      }
    }

    if (tmp) {
      ToWaitNode(tmp)->Notify();
    }
  }

  void AbortAll() {
    AtomicFenceSeqCst();
    AbortAllRelaxed();
  }

  void AbortAllRelaxed() {
    if (waitset_.Empty()) {
      return;
    }

    base_list temp;
    const base_node* end;
    {
      ConcurrentMonitorMutex::scoped_lock l(mutex_);
      epoch_.store(epoch_.load(std::memory_order_relaxed) + 1,
                   std::memory_order_relaxed);
      waitset_.FlushTo(temp);
      end = temp.End();
      for (base_node* n = temp.Front(); n != end; n = n->next) {
        ToWaitNode(n)->is_in_list_.store(false, std::memory_order_relaxed);
      }
    }

    base_node* nxt;
    for (base_node* n = temp.Front(); n != end; n = nxt) {
      nxt = n->next;
      ToWaitNode(n)->aborted_ = true;
      ToWaitNode(n)->Notify();
    }
  }

  void Destroy() {
    this->AbortAll();
    mutex_.destroy();
    assert(this->waitset_.Empty() && "waitset not empty?");
  }

 private:
  template <typename TNodeType, typename TPred>
  bool GuardedCall(TPred&& predicate, TNodeType& node) {
    bool res = false;
    ws::concurrency::detail::templates::TryCall([&] {
      res = std::forward<TPred>(predicate)();
    }).OnException([&] { CancelWait(node); });

    return res;
  }

  ConcurrentMonitorMutex mutex_{};
  base_list waitset_{};
  std::atomic<unsigned> epoch_{};

  WaitNode<TContext>* ToWaitNode(base_node* node) {
    return static_cast<WaitNode<TContext>*>(node);
  }
};

class ConcurrentMonitor : public ConcurrentMonitorBase<std::uintptr_t> {
  using base_type = ConcurrentMonitorBase<std::uintptr_t>;

 public:
  using base_type::base_type;

  ~ConcurrentMonitor() { Destroy(); }

  using thread_context = SleepNode<std::uintptr_t>;
};

}  // namespace detail
}  // namespace concurrency
}  // namespace ws
