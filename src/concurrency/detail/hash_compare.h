#pragma once
#include <functional>

#include "arch/config.h"
#include "concurrency/detail/helpers.h"

namespace ws {
namespace concurrency {
namespace detail {

template <typename TKey, typename THash, typename TKeyEqual>
class HashCompare {
  using is_transparent_hash =
      ws::concurrency::detail::containers::HasTransparentKeyEqual<TKey, THash,
                                                                  TKeyEqual>;

 public:
  using hasher = THash;
  using key_equal = typename is_transparent_hash::type;

  HashCompare() = default;
  HashCompare(hasher hash, key_equal equal) : hasher_(hash), equal_(equal) {}

  std::size_t operator()(const TKey& key) const {
    return std::size_t(hasher_(key));
  }

  bool operator()(const TKey& key1, const TKey& key2) const {
    return equal_(key1, key2);
  }

  template <typename TKey1, typename = typename std::enable_if<
                                is_transparent_hash::value, TKey1>::type>
  std::size_t operator()(const TKey1& key) const {
    return std::size_t(hasher_(key));
  }

  template <typename TKey1, typename TKey2,
            typename = typename std::enable_if<is_transparent_hash::value,
                                               TKey1>::type>
  bool operator()(const TKey1& key1, const TKey2& key2) const {
    return equal_(key1, key2);
  }

  hasher HashFunction() const { return hasher_; }

  key_equal KeyEq() const { return equal_; }

 private:
  hasher hasher_;
  key_equal equal_;
};

template <typename TKey>
class WsHashCompare {
 public:
  std::size_t Hash(const TKey& a) const { return hash_func_(a); }

#if defined(_MSC_VER) && _MSC_VER <= 1900
#pragma warning(push)
#pragma warning(disable : 4800)
#endif

  bool Equal(const TKey& a, const TKey& b) const { return key_equal_(a, b); }

#if defined(_MSC_VER) && _MSC_VER <= 1900
#pragma warning(pop)
#endif

 private:
  std::hash<TKey> hash_func_;
  std::equal_to<TKey> key_equal_;
};

#if ((__cpp_concepts >= 201907L) && (__cpp_lib_concepts >= 202002L))
template <typename THashCompare, typename TKey>
concept HashCompareConcept =
    std::copy_constructible<THashCompare> &&
    requires(const std::remove_reference_t<THashCompare>& hc, const TKey& key1,
             const TKey& key2) {
      { hc.Hash(key1) } -> std::same_as<std::size_t>;
      { hc.Equal(key1, key2) } -> std::convertible_to<bool>;
    };
#endif

}  // namespace detail
}  // namespace concurrency
}  // namespace ws
