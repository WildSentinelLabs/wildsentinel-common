#pragma once

#include "arch/config.h"
#include "arch/cpu_arch.h"

namespace ws {
namespace concurrency {
namespace detail {
namespace templates {

template <typename TFunc>
class RaiiGuard {
 public:
  static_assert(std::is_nothrow_copy_constructible<TFunc>::value &&
                    std::is_nothrow_move_constructible<TFunc>::value,
                "Throwing an exception during the Func copy or move "
                "construction cause an unexpected behavior.");

  RaiiGuard(TFunc f) noexcept : func_(f), is_active_(true) {}

  RaiiGuard(RaiiGuard&& g) noexcept
      : func_(std::move(g.func_)), is_active_(g.is_active_) {
    g.is_active_ = false;
  }

  ~RaiiGuard() {
    if (is_active_) {
      func_();
    }
  }

  void Dismiss() { is_active_ = false; }

 private:
  TFunc func_;
  bool is_active_;
};

template <typename TFunc>
RaiiGuard<TFunc> MakeRaiiGuard(TFunc f) {
  return RaiiGuard<TFunc>(f);
}

template <typename TBody>
struct TryCallProxy {
  TryCallProxy(TBody b) : body(b) {}

  template <typename TOnExceptionBody>
  void OnException(TOnExceptionBody on_exception_body) {
    auto guard =
        ws::concurrency::detail::templates::MakeRaiiGuard(on_exception_body);
    body();
    guard.Dismiss();
  }

  template <typename TOnCompletionBody>
  void OnCompletion(TOnCompletionBody on_completion_body) {
    auto guard =
        ws::concurrency::detail::templates::MakeRaiiGuard(on_completion_body);
    body();
  }

  TBody body;
};

template <typename TBody>
inline TryCallProxy<TBody> TryCall(TBody b) {
  return TryCallProxy<TBody>(b);
}

template <typename...>
struct VoidImpl {
  using type = void;
};

template <typename... TArgs>
using void_t = typename VoidImpl<TArgs...>::type;

template <typename T, typename, template <typename> class... Checks>
struct SupportsImpl {
  using type = std::false_type;
};

template <typename T, template <typename> class... Checks>
struct SupportsImpl<T, void_t<Checks<T>...>, Checks...> {
  using type = std::true_type;
};

template <typename T, template <typename> class... Checks>
using supports = typename SupportsImpl<T, void, Checks...>::type;

template <typename TIterator>
using iterator_value_t = typename std::iterator_traits<TIterator>::value_type;

template <typename TIterator>
using iterator_key_t = typename std::remove_const<
    typename iterator_value_t<TIterator>::first_type>::type;

template <typename TIterator>
using iterator_mapped_t = typename iterator_value_t<TIterator>::second_type;

template <typename TIterator>
using iterator_alloc_pair_t =
    std::pair<typename std::add_const<iterator_key_t<TIterator>>::type,
              iterator_mapped_t<TIterator>>;

template <typename TIterator>
using iterator_alloc_pair_t =
    std::pair<typename std::add_const<iterator_key_t<TIterator>>::type,
              iterator_mapped_t<TIterator>>;

template <typename TA>
using alloc_value_type = typename TA::value_type;
template <typename TA>
using alloc_ptr_t = typename std::allocator_traits<TA>::pointer;
template <typename TA>
using has_allocate =
    decltype(std::declval<alloc_ptr_t<TA>&>() = std::declval<TA>().allocate(0));
template <typename TA>
using has_deallocate =
    decltype(std::declval<TA>().deallocate(std::declval<alloc_ptr_t<TA>>(), 0));

template <typename T>
using is_allocator =
    supports<T, alloc_value_type, has_allocate, has_deallocate>;

#if _CPP17_DEDUCTION_GUIDES_PRESENT
template <typename T>
inline constexpr bool is_allocator_v = is_allocator<T>::value;
#endif

template <typename T, typename TU>
inline T punned_cast(TU* ptr) {
  std::uintptr_t x = reinterpret_cast<std::uintptr_t>(ptr);
  return reinterpret_cast<T>(x);
}

}  // namespace templates

namespace containers {

template <typename TCompare, typename = void>
struct CompIsTransparent : std::false_type {};

template <typename TCompare>
struct CompIsTransparent<TCompare, ws::concurrency::detail::templates::void_t<
                                       typename TCompare::is_transparent>>
    : std::true_type {};

template <typename TKey, typename THasher, typename TKeyEqual, typename = void>
struct HasTransparentKeyEqual : std::false_type {
  using type = TKeyEqual;
};

template <typename TKey, typename THasher, typename TKeyEqual>
struct HasTransparentKeyEqual<TKey, THasher, TKeyEqual,
                              ws::concurrency::detail::templates::void_t<
                                  typename THasher::transparent_key_equal>>
    : std::true_type {
  using type = typename THasher::transparent_key_equal;
  static_assert(CompIsTransparent<type>::value,
                "Hash::transparent_key_equal::is_transparent is not valid or "
                "does not denote a type.");
  static_assert(
      (std::is_same<TKeyEqual, std::equal_to<TKey>>::value ||
       std::is_same<typename THasher::transparent_key_equal, TKeyEqual>::value),
      "KeyEqual is a different type than equal_to<Key> or "
      "Hash::transparent_key_equal.");
};

template <typename TTrait, typename T>
struct DependentBool : std::integral_constant<bool, bool(TTrait::value)> {};

struct IsIteratorImpl {
  template <typename T>
  using iter_traits_category =
      typename std::iterator_traits<T>::iterator_category;

  template <typename T>
  using input_iter_category = typename std::enable_if<std::is_base_of<
      std::input_iterator_tag, iter_traits_category<T>>::value>::type;
};

template <typename T>
using is_input_iterator = ws::concurrency::detail::templates::supports<
    T, IsIteratorImpl::iter_traits_category,
    IsIteratorImpl::input_iter_category>;

#if _CPP17_DEDUCTION_GUIDES_PRESENT
template <typename T>
inline constexpr bool is_input_iterator_v = is_input_iterator<T>::value;
#endif

}  // namespace containers

namespace range {
class split {};
}  // namespace range

template <typename T>
std::uintptr_t Log2(T in) {
  assert((in > 0) && "The logarithm of a non-positive value is undefined.");
  return ws::detail::CpuLog2(in);
}

template <typename T>
T ReverseBits(T src) {
  return ws::detail::CpuReverseBits(src);
}

template <typename T>
T ReverseNBits(T src, std::size_t n) {
  assert(n != 0 && "Reverse for 0 bits is undefined behavior.");
  return ReverseBits(src) >> (ws::detail::NumberOfBits<T>() - n);
}

}  // namespace detail
}  // namespace concurrency
}  // namespace ws
