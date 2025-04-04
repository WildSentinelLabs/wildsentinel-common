#pragma once

#include <cstddef>
#include <cstring>
#include <iostream>
#include <new>
#include <stdexcept>
#include <type_traits>
#include <utility>

#include "arch/cpu_arch.h"

namespace ws {

struct bad_delegate_call_exception : public std::exception {
  const char* what() const noexcept override { return "Bad delegate call"; }
};

template <typename Signature>
class Delegate;

template <typename R, typename... Args>
class Delegate<R(Args...)> {
 public:
  Delegate() noexcept = default;
  Delegate(std::nullptr_t) noexcept : Delegate() {}

  template <typename F, typename = std::enable_if_t<
                            !std::is_same<std::decay_t<F>, Delegate>::value>>
  Delegate(F&& f) {
    Assign(std::forward<F>(f));
  }

  template <typename T, typename MemFunc,
            typename = std::enable_if_t<
                std::is_member_function_pointer<MemFunc>::value>>
  Delegate(T* object, MemFunc memFunc) {
    Assign([object, memFunc](Args... args) -> R {
      return (object->*memFunc)(args...);
    });
  }

  Delegate(const Delegate& other) {
    if (other.manager_fn_) {
      other.manager_fn_(COPY_OP, static_cast<const void*>(other.storage_),
                        static_cast<void*>(storage_));
      call_fn_ = other.call_fn_;
      manager_fn_ = other.manager_fn_;
      using_sbo_ = other.using_sbo_;
    }
  }

  Delegate(Delegate&& other) noexcept {
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

  Delegate& operator=(const Delegate& other) {
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

  Delegate& operator=(Delegate&& other) noexcept {
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
          other.manager_fn_(DESTROY_OP,
                            static_cast<const void*>(other.storage_), nullptr);
        }
        other.call_fn_ = nullptr;
        other.manager_fn_ = nullptr;
      }
    }
    return *this;
  }

  ~Delegate() { Reset(); }

  void Reset() {
    if (manager_fn_) {
      manager_fn_(DESTROY_OP, static_cast<const void*>(storage_), nullptr);
      call_fn_ = nullptr;
      manager_fn_ = nullptr;
      using_sbo_ = true;
    }
  }

  R operator()(Args... args) const {
    if (!call_fn_) throw bad_delegate_call_exception();
    return call_fn_(static_cast<const void*>(storage_),
                    std::forward<Args>(args)...);
  }

  explicit operator bool() const noexcept { return call_fn_ != nullptr; }

 private:
  static constexpr std::size_t kSboSize = 64;

  alignas(ws::arch::detail::CacheLineSize()) std::uint8_t storage_[kSboSize];

  using CallFn = R (*)(const void*, Args&&...);

  using ManagerFn = void (*)(int op, const void* src, void* dest);
  enum ManagerOp { COPY_OP, MOVE_OP, DESTROY_OP };

  CallFn call_fn_ = nullptr;
  ManagerFn manager_fn_ = nullptr;
  bool using_sbo_ = true;

  template <typename F>
  using UseSBO =
      std::integral_constant<bool,
                             (sizeof(F) <= kSboSize &&
                              std::is_nothrow_move_constructible<F>::value)>;

  template <typename F, bool UseInline>
  struct FunctionManager;

  template <typename F>
  struct FunctionManager<F, true> {
    static R call(const void* storage, Args&&... args) {
      const F* f = reinterpret_cast<const F*>(storage);
      return (*f)(std::forward<Args>(args)...);
    }
    static void manager(int op, const void* src, void* dest) {
      if (op == COPY_OP) {
        new (dest) F(*reinterpret_cast<const F*>(src));
      } else if (op == MOVE_OP) {
        new (dest) F(std::move(*reinterpret_cast<const F*>(src)));
      } else if (op == DESTROY_OP) {
        reinterpret_cast<const F*>(src)->~F();
      }
    }
  };

  template <typename F>
  struct FunctionManager<F, false> {
    static R call(const void* storage, Args&&... args) {
      F* const* fptr = reinterpret_cast<F* const*>(storage);
      return (**fptr)(std::forward<Args>(args)...);
    }
    static void manager(int op, const void* src, void* dest) {
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
  };

  template <typename F>
  void Assign(F&& f) {
    using DecayedF = std::decay_t<F>;
    constexpr bool useInline = UseSBO<DecayedF>::value;
    call_fn_ = &FunctionManager<DecayedF, useInline>::call;
    manager_fn_ = &FunctionManager<DecayedF, useInline>::manager;
    using_sbo_ = useInline;
    if (using_sbo_) {
      new (static_cast<void*>(storage_)) DecayedF(std::forward<F>(f));
    } else {
      DecayedF* ptr = new DecayedF(std::forward<F>(f));
      std::memcpy(static_cast<void*>(storage_), &ptr, sizeof(ptr));
    }
  }
};

}  // namespace ws
