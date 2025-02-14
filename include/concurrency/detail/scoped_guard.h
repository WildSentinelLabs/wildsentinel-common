#pragma once

namespace ws {
namespace concurrency {
namespace detail {

template <typename Func>
class raii_guard {
 public:
  static_assert(std::is_nothrow_copy_constructible<Func>::value &&
                    std::is_nothrow_move_constructible<Func>::value,
                "Throwing an exception during the Func copy or move "
                "construction cause an unexpected behavior.");

  raii_guard(Func f) noexcept : my_func(f), is_active(true) {}

  raii_guard(raii_guard&& g) noexcept
      : my_func(std::move(g.my_func)), is_active(g.is_active) {
    g.is_active = false;
  }

  ~raii_guard() {
    if (is_active) {
      my_func();
    }
  }

  void dismiss() { is_active = false; }

 private:
  Func my_func;
  bool is_active;
};

template <typename Func>
raii_guard<Func> make_raii_guard(Func f) {
  return raii_guard<Func>(f);
}

template <typename Body>
struct try_call_proxy {
  try_call_proxy(Body b) : body(b) {}

  template <typename OnExceptionBody>
  void on_exception(OnExceptionBody on_exception_body) {
    auto guard = make_raii_guard(on_exception_body);
    body();
    guard.dismiss();
  }

  template <typename OnCompletionBody>
  void on_completion(OnCompletionBody on_completion_body) {
    auto guard = make_raii_guard(on_completion_body);
    body();
  }

  Body body;
};

template <typename Body>
inline try_call_proxy<Body> try_call(Body b) {
  return try_call_proxy<Body>(b);
}
}  // namespace detail
}  // namespace concurrency
}  // namespace ws
