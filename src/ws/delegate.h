#pragma once

#include <cstddef>
#include <cstring>
#include <new>
#include <type_traits>
#include <utility>

#include "ws/config.h"
#include "ws/machine.h"

namespace ws {
struct bad_delegate_call_exception : public std::exception {
  const char* what() const noexcept override;
};

template <typename Signature>
class Delegate;

template <typename R, typename... Args>
class Delegate<R(Args...)> {
 public:
  Delegate() noexcept = default;
  Delegate(std::nullptr_t) noexcept;

  template <typename F, typename = std::enable_if_t<
                            !std::is_same<std::decay_t<F>, Delegate>::value>>
  Delegate(F&& f);

  template <typename T, typename MemFunc,
            typename = std::enable_if_t<
                std::is_member_function_pointer<MemFunc>::value>>
  Delegate(T* object, MemFunc memFunc);

  Delegate(const Delegate&);
  Delegate(Delegate&&) noexcept;

  Delegate& operator=(const Delegate&);
  Delegate& operator=(Delegate&&) noexcept;
  R operator()(Args... args) const;
  explicit operator bool() const noexcept;

  ~Delegate();

  void Reset();

 private:
  static constexpr std::size_t kSboSize = 64;

 private:
  using CallFn = R (*)(const void*, Args&&...);
  using ManagerFn = void (*)(int op, const void* src, void* dest);

  template <typename F>
  using UseSBO =
      std::integral_constant<bool,
                             (sizeof(F) <= kSboSize &&
                              std::is_nothrow_move_constructible<F>::value)>;

  enum ManagerOp { COPY_OP, MOVE_OP, DESTROY_OP };

  template <typename F, bool UseInline>
  struct FunctionManager;

  template <typename F>
  struct FunctionManager<F, true> {
    static R Call(const void* storage, Args&&... args);
    static void Manager(int op, const void* src, void* dest);
  };

  template <typename F>
  struct FunctionManager<F, false> {
    static R Call(const void* storage, Args&&... args);
    static void Manager(int op, const void* src, void* dest);
  };

  template <typename F>
  void Assign(F&& f);

  alignas(ws::internal::CacheLineSize()) std::uint8_t storage_[kSboSize];
  CallFn call_fn_ = nullptr;
  ManagerFn manager_fn_ = nullptr;
  bool using_sbo_ = true;
};

// ============================================================================
// Implementation details for bad_delegate_call_exception
// ============================================================================
inline const char* bad_delegate_call_exception::what() const noexcept {
  return "Bad delegate call";
}

// ============================================================================
// Implementation details for Delegate<R(Args...)>
// ============================================================================

template <typename R, typename... Args>
inline Delegate<R(Args...)>::Delegate(::std::nullptr_t) noexcept : Delegate() {}

template <typename R, typename... Args>
template <typename F, typename>
inline Delegate<R(Args...)>::Delegate(F&& f) {
  Assign(std::forward<F>(f));
}

template <typename R, typename... Args>
template <typename T, typename MemFunc, typename>
inline Delegate<R(Args...)>::Delegate(T* object, MemFunc memFunc) {
  Assign([object, memFunc](Args... args) -> R {
    return (object->*memFunc)(args...);
  });
}

template <typename R, typename... Args>
inline Delegate<R(Args...)>::Delegate(const Delegate& other) {
  if (other.manager_fn_) {
    other.manager_fn_(COPY_OP, static_cast<const void*>(other.storage_),
                      static_cast<void*>(storage_));
    call_fn_ = other.call_fn_;
    manager_fn_ = other.manager_fn_;
    using_sbo_ = other.using_sbo_;
  }
}

template <typename R, typename... Args>
inline Delegate<R(Args...)>::Delegate(Delegate&& other) noexcept {
  if (other.manager_fn_) {
    call_fn_ = other.call_fn_;
    manager_fn_ = other.manager_fn_;
    using_sbo_ = other.using_sbo_;
    if (using_sbo_) {
      manager_fn_(MOVE_OP, static_cast<const void*>(other.storage_),
                  static_cast<void*>(storage_));
    } else {
      std::memcpy(static_cast<void*>(storage_),
                  static_cast<const void*>(other.storage_), sizeof(void*));
      other.manager_fn_(DESTROY_OP, static_cast<const void*>(other.storage_),
                        nullptr);
    }
    other.call_fn_ = nullptr;
    other.manager_fn_ = nullptr;
  }
}

template <typename R, typename... Args>
inline Delegate<R(Args...)>& Delegate<R(Args...)>::operator=(
    const Delegate& other) {
  if (this != &other) {
    Reset();
    if (other.manager_fn_) {
      other.manager_fn_(COPY_OP, static_cast<const void*>(other.storage_),
                        static_cast<void*>(storage_));
      call_fn_ = other.call_fn_;
      manager_fn_ = other.manager_fn_;
      using_sbo_ = other.using_sbo_;
    }
  }
  return *this;
}

template <typename R, typename... Args>
inline Delegate<R(Args...)>& Delegate<R(Args...)>::operator=(
    Delegate&& other) noexcept {
  if (this != &other) {
    Reset();
    if (other.manager_fn_) {
      call_fn_ = other.call_fn_;
      manager_fn_ = other.manager_fn_;
      using_sbo_ = other.using_sbo_;
      if (using_sbo_) {
        manager_fn_(MOVE_OP, static_cast<const void*>(other.storage_),
                    static_cast<void*>(storage_));
      } else {
        std::memcpy(static_cast<void*>(storage_),
                    static_cast<const void*>(other.storage_), sizeof(void*));
        other.manager_fn_(DESTROY_OP, static_cast<const void*>(other.storage_),
                          nullptr);
      }
      other.call_fn_ = nullptr;
      other.manager_fn_ = nullptr;
    }
  }
  return *this;
}

template <typename R, typename... Args>
inline R Delegate<R(Args...)>::operator()(Args... args) const {
  if (!call_fn_) throw bad_delegate_call_exception();
  return call_fn_(static_cast<const void*>(storage_),
                  std::forward<Args>(args)...);
}

template <typename R, typename... Args>
inline Delegate<R(Args...)>::operator bool() const noexcept {
  return call_fn_ != nullptr;
}

template <typename R, typename... Args>
inline Delegate<R(Args...)>::~Delegate() {
  Reset();
}

template <typename R, typename... Args>
inline void Delegate<R(Args...)>::Reset() {
  if (manager_fn_) {
    manager_fn_(DESTROY_OP, static_cast<const void*>(storage_), nullptr);
    call_fn_ = nullptr;
    manager_fn_ = nullptr;
    using_sbo_ = true;
  }
}

template <typename R, typename... Args>
template <typename F>
R Delegate<R(Args...)>::FunctionManager<F, true>::Call(const void* storage,
                                                       Args&&... args) {
  const F* f = reinterpret_cast<const F*>(storage);
  return (*f)(std::forward<Args>(args)...);
}

template <typename R, typename... Args>
template <typename F>
void Delegate<R(Args...)>::FunctionManager<F, true>::Manager(int op,
                                                             const void* src,
                                                             void* dest) {
  if (op == COPY_OP) {
    new (dest) F(*reinterpret_cast<const F*>(src));
  } else if (op == MOVE_OP) {
    new (dest) F(std::move(*reinterpret_cast<const F*>(src)));
  } else if (op == DESTROY_OP) {
    reinterpret_cast<const F*>(src)->~F();
  }
}

template <typename R, typename... Args>
template <typename F>
R Delegate<R(Args...)>::FunctionManager<F, false>::Call(const void* storage,
                                                        Args&&... args) {
  F* const* fptr = reinterpret_cast<F* const*>(storage);
  return (**fptr)(std::forward<Args>(args)...);
}

template <typename R, typename... Args>
template <typename F>
void Delegate<R(Args...)>::FunctionManager<F, false>::Manager(int op,
                                                              const void* src,
                                                              void* dest) {
  if (op == COPY_OP) {
    F* const* src_ptr = reinterpret_cast<F* const*>(src);
    F* copy = new F(**src_ptr);
    std::memcpy(dest, &copy, sizeof(copy));
  } else if (op == MOVE_OP) {
    F** src_ptr = reinterpret_cast<F**>(const_cast<void*>(src));
    std::memcpy(dest, src_ptr, sizeof(*src_ptr));
    *src_ptr = nullptr;
  } else if (op == DESTROY_OP) {
    F* const* fptr = reinterpret_cast<F* const*>(src);
    delete *fptr;
  }
}

template <typename R, typename... Args>
template <typename F>
void Delegate<R(Args...)>::Assign(F&& f) {
  using DecayedF = std::decay_t<F>;
  constexpr bool useInline = UseSBO<DecayedF>::value;
  call_fn_ = &FunctionManager<DecayedF, useInline>::Call;
  manager_fn_ = &FunctionManager<DecayedF, useInline>::Manager;
  using_sbo_ = useInline;
  if (using_sbo_) {
    new (static_cast<void*>(storage_)) DecayedF(std::forward<F>(f));
  } else {
    DecayedF* ptr = new DecayedF(std::forward<F>(f));
    std::memcpy(static_cast<void*>(storage_), &ptr, sizeof(ptr));
  }
}
}  // namespace ws
