#pragma once

#include "concurrency/detail/concurrent_unordered_base.h"
#include "concurrency/detail/helpers.h"

namespace ws {
namespace concurrency {
namespace collections {

template <typename TKey, typename THash, typename TKeyEqual,
          typename TAllocator, bool AllowMultimapping>
struct ConcurrentUnorderedSetTraits {
  using key_type = TKey;
  using value_type = key_type;
  using allocator_type = TAllocator;
  using hash_compare_type =
      ws::concurrency::detail::HashCompare<key_type, THash, TKeyEqual>;
  static constexpr bool kAllowMultimapping = AllowMultimapping;

  static constexpr const key_type& GetKey(const value_type& value) {
    return value;
  }
};

template <typename TKey, typename THash = std::hash<TKey>,
          typename TKeyEqual = std::equal_to<TKey>,
          typename TAllocator = std::allocator<TKey>>
class ConcurrentUnorderedSet
    : public ws::concurrency::detail::ConcurrentUnorderedBase<
          ConcurrentUnorderedSetTraits<TKey, THash, TKeyEqual, TAllocator,
                                       false>> {
  using traits_type =
      ConcurrentUnorderedSetTraits<TKey, THash, TKeyEqual, TAllocator, false>;
  using base_type =
      ws::concurrency::detail::ConcurrentUnorderedBase<traits_type>;

 public:
  using key_type = typename base_type::key_type;
  using value_type = typename base_type::value_type;
  using size_type = typename base_type::size_type;
  using difference_type = typename base_type::difference_type;
  using hasher = typename base_type::hasher;
  using key_equal = typename base_type::key_equal;
  using allocator_type = typename base_type::allocator_type;
  using reference = typename base_type::reference;
  using const_reference = typename base_type::const_reference;
  using pointer = typename base_type::pointer;
  using const_pointer = typename base_type::const_pointer;
  using iterator = typename base_type::iterator;
  using const_iterator = typename base_type::const_iterator;
  using local_iterator = typename base_type::local_iterator;
  using const_local_iterator = typename base_type::const_local_iterator;
  using node_type = typename base_type::node_type;

  using base_type::base_type;

  ConcurrentUnorderedSet() = default;
  ConcurrentUnorderedSet(const ConcurrentUnorderedSet&) = default;
  ConcurrentUnorderedSet(const ConcurrentUnorderedSet& other,
                         const allocator_type& alloc)
      : base_type(other, alloc) {}
  ConcurrentUnorderedSet(ConcurrentUnorderedSet&&) = default;
  ConcurrentUnorderedSet(ConcurrentUnorderedSet&& other,
                         const allocator_type& alloc)
      : base_type(std::move(other), alloc) {}

  ConcurrentUnorderedSet& operator=(const ConcurrentUnorderedSet&) = default;
  ConcurrentUnorderedSet& operator=(ConcurrentUnorderedSet&&) = default;

  ConcurrentUnorderedSet& operator=(std::initializer_list<value_type> il) {
    base_type::operator=(il);
    return *this;
  }

  template <typename TOtherHash, typename TOtherKeyEqual>
  void Merge(ConcurrentUnorderedSet<key_type, TOtherHash, TOtherKeyEqual,
                                    allocator_type>& source) {
    this->InternalMerge(source);
  }

  template <typename TOtherHash, typename TOtherKeyEqual>
  void Merge(ConcurrentUnorderedSet<key_type, TOtherHash, TOtherKeyEqual,
                                    allocator_type>&& source) {
    this->InternalMerge(std::move(source));
  }
};

#if _CPP17_DEDUCTION_GUIDES_PRESENT

template <
    typename It,
    typename THash =
        std::hash<ws::concurrency::detail::templates::iterator_value_t<It>>,
    typename TKeyEq =
        std::equal_to<ws::concurrency::detail::templates::iterator_value_t<It>>,
    typename TAlloc = std::allocator<
        ws::concurrency::detail::templates::iterator_value_t<It>>,
    typename = std::enable_if_t<
        ws::concurrency::detail::containers::is_input_iterator_v<It>>,
    typename = std::enable_if_t<
        ws::concurrency::detail::templates::is_allocator_v<TAlloc>>,
    typename = std::enable_if_t<
        !ws::concurrency::detail::templates::is_allocator_v<THash>>,
    typename = std::enable_if_t<
        !ws::concurrency::detail::templates::is_allocator_v<TKeyEq>>,
    typename = std::enable_if_t<!std::is_integral_v<THash>>>
ConcurrentUnorderedSet(It, It, std::size_t = {}, THash = THash(),
                       TKeyEq = TKeyEq(), TAlloc = TAlloc())
    -> ConcurrentUnorderedSet<
        ws::concurrency::detail::templates::iterator_value_t<It>, THash, TKeyEq,
        TAlloc>;

template <typename T, typename THash = std::hash<T>,
          typename TKeyEq = std::equal_to<T>,
          typename TAlloc = std::allocator<T>,
          typename = std::enable_if_t<
              ws::concurrency::detail::templates::is_allocator_v<TAlloc>>,
          typename = std::enable_if_t<
              !ws::concurrency::detail::templates::is_allocator_v<THash>>,
          typename = std::enable_if_t<
              !ws::concurrency::detail::templates::is_allocator_v<TKeyEq>>,
          typename = std::enable_if_t<!std::is_integral_v<THash>>>
ConcurrentUnorderedSet(std::initializer_list<T>, std::size_t = {},
                       THash = THash(), TKeyEq = TKeyEq(), TAlloc = TAlloc())
    -> ConcurrentUnorderedSet<T, THash, TKeyEq, TAlloc>;

template <typename TIt, typename TAlloc,
          typename = std::enable_if_t<
              ws::concurrency::detail::containers::is_input_iterator_v<TIt>>,
          typename = std::enable_if_t<
              ws::concurrency::detail::templates::is_allocator_v<TAlloc>>>
ConcurrentUnorderedSet(TIt, TIt, std::size_t, TAlloc) -> ConcurrentUnorderedSet<
    ws::concurrency::detail::templates::iterator_value_t<TIt>,
    std::hash<ws::concurrency::detail::templates::iterator_value_t<TIt>>,
    std::equal_to<ws::concurrency::detail::templates::iterator_value_t<TIt>>,
    TAlloc>;

template <typename TIt, typename THash, typename Alloc,
          typename = std::enable_if_t<
              ws::concurrency::detail::containers::is_input_iterator_v<TIt>>,
          typename = std::enable_if_t<
              ws::concurrency::detail::templates::is_allocator_v<Alloc>>,
          typename = std::enable_if_t<
              !ws::concurrency::detail::templates::is_allocator_v<THash>>,
          typename = std::enable_if_t<!std::is_integral_v<THash>>>
ConcurrentUnorderedSet(TIt, TIt, std::size_t, THash, Alloc)
    -> ConcurrentUnorderedSet<
        ws::concurrency::detail::templates::iterator_value_t<TIt>, THash,
        std::equal_to<
            ws::concurrency::detail::templates::iterator_value_t<TIt>>,
        Alloc>;

template <typename T, typename TAlloc,
          typename = std::enable_if_t<
              ws::concurrency::detail::templates::is_allocator_v<TAlloc>>>
ConcurrentUnorderedSet(std::initializer_list<T>, std::size_t, TAlloc)
    -> ConcurrentUnorderedSet<T, std::hash<T>, std::equal_to<T>, TAlloc>;

template <typename T, typename TAlloc,
          typename = std::enable_if_t<
              ws::concurrency::detail::templates::is_allocator_v<TAlloc>>>
ConcurrentUnorderedSet(std::initializer_list<T>, TAlloc)
    -> ConcurrentUnorderedSet<T, std::hash<T>, std::equal_to<T>, TAlloc>;

template <typename T, typename Hash, typename TAlloc,
          typename = std::enable_if_t<
              ws::concurrency::detail::templates::is_allocator_v<TAlloc>>,
          typename = std::enable_if_t<
              !ws::concurrency::detail::templates::is_allocator_v<Hash>>,
          typename = std::enable_if_t<!std::is_integral_v<Hash>>>
ConcurrentUnorderedSet(std::initializer_list<T>, std::size_t, Hash, TAlloc)
    -> ConcurrentUnorderedSet<T, Hash, std::equal_to<T>, TAlloc>;

#endif

template <typename TKey, typename THash, typename TKeyEqual,
          typename TAllocator>
void Swap(ConcurrentUnorderedSet<TKey, THash, TKeyEqual, TAllocator>& lhs,
          ConcurrentUnorderedSet<TKey, THash, TKeyEqual, TAllocator>& rhs) {
  lhs.Swap(rhs);
}

namespace stl {

template <typename TKey, typename THash = std::hash<TKey>,
          typename TKeyEqual = std::equal_to<TKey>,
          typename TAllocator = std::allocator<TKey>>
class concurrent_unordered_set {
 public:
  using hashtable_type =
      ConcurrentUnorderedSet<TKey, THash, TKeyEqual, TAllocator>;
  using key_type = typename hashtable_type::key_type;
  using mapped_type = typename hashtable_type::mapped_type;
  using value_type = typename hashtable_type::value_type;
  using size_type = typename hashtable_type::size_type;
  using difference_type = typename hashtable_type::difference_type;
  using hasher = typename hashtable_type::hasher;
  using key_equal = typename hashtable_type::key_equal;
  using allocator_type = typename hashtable_type::allocator_type;
  using reference = typename hashtable_type::reference;
  using const_reference = typename hashtable_type::const_reference;
  using pointer = typename hashtable_type::pointer;
  using const_pointer = typename hashtable_type::const_pointer;
  using iterator = typename hashtable_type::iterator;
  using const_iterator = typename hashtable_type::const_iterator;
  using local_iterator = typename hashtable_type::local_iterator;
  using const_local_iterator = typename hashtable_type::const_local_iterator;
  using node_type = typename hashtable_type::node_type;

 private:
  hashtable_type internal_instance_;

 public:
  concurrent_unordered_set() = default;

  explicit concurrent_unordered_set(size_type n, const hasher& hf = hasher(),
                                    const key_equal& eql = key_equal(),
                                    const allocator_type& a = allocator_type())
      : internal_instance_(n, hf, eql, a) {}

  template <typename TInputIterator>
  concurrent_unordered_set(TInputIterator first, TInputIterator last,
                           size_type n = 0, const hasher& hf = hasher(),
                           const key_equal& eql = key_equal(),
                           const allocator_type& a = allocator_type())
      : internal_instance_(first, last, n, hf, eql, a) {}

  concurrent_unordered_set(const concurrent_unordered_set&) = default;

  concurrent_unordered_set(concurrent_unordered_set&&) = default;

  explicit concurrent_unordered_set(const allocator_type& a)
      : internal_instance_(a) {}

  concurrent_unordered_set(const concurrent_unordered_set& other,
                           const allocator_type& a)
      : internal_instance_(other.internal_instance_, a) {}

  concurrent_unordered_set(
      concurrent_unordered_set&& other,
      const allocator_type&
          a) noexcept(noexcept(hashtable_type(std::
                                                  move(
                                                      other.internal_instance_),
                                              a)))
      : internal_instance_(std::move(other.internal_instance_), a) {}

  concurrent_unordered_set(std::initializer_list<value_type> il,
                           size_type n = 0, const hasher& hf = hasher(),
                           const key_equal& eql = key_equal(),
                           const allocator_type& a = allocator_type())
      : internal_instance_(il, n, hf, eql, a) {}

  concurrent_unordered_set(size_type n, const allocator_type& a)
      : concurrent_unordered_set(n, hasher(), key_equal(), a) {}

  concurrent_unordered_set(size_type n, const hasher& hf,
                           const allocator_type& a)
      : concurrent_unordered_set(n, hf, key_equal(), a) {}

  template <typename TInputIterator>
  concurrent_unordered_set(TInputIterator first, TInputIterator last,
                           size_type n, const allocator_type& a)
      : concurrent_unordered_set(first, last, n, hasher(), key_equal(), a) {}

  template <typename TInputIterator>
  concurrent_unordered_set(TInputIterator first, TInputIterator last,
                           size_type n, const hasher& hf,
                           const allocator_type& a)
      : concurrent_unordered_set(first, last, n, hf, key_equal(), a) {}

  concurrent_unordered_set(std::initializer_list<value_type> il, size_type n,
                           const allocator_type& a)
      : concurrent_unordered_set(il, n, hasher(), key_equal(), a) {}

  concurrent_unordered_set(std::initializer_list<value_type> il, size_type n,
                           const hasher& hf, const allocator_type& a)
      : concurrent_unordered_set(il, n, hf, key_equal(), a) {}

  concurrent_unordered_set& operator=(const concurrent_unordered_set&) =
      default;
  concurrent_unordered_set& operator=(concurrent_unordered_set&&) = default;

  concurrent_unordered_set& operator=(std::initializer_list<value_type> il) {
    internal_instance_ = il;
    return *this;
  }

  allocator_type get_allocator() const noexcept {
    return internal_instance_.get_allocator();
  }

  [[nodiscard]] bool empty() const noexcept {
    return internal_instance_.Empty();
  }

  size_type size() const noexcept { return internal_instance_.Size(); }

  size_type max_size() const noexcept { return internal_instance_.MaxSize(); }

  iterator begin() noexcept { return internal_instance_.begin(); }

  const_iterator begin() const noexcept { return internal_instance_.begin(); }

  iterator end() noexcept { return internal_instance_.end(); }

  const_iterator end() const noexcept { return internal_instance_.end(); }

  const_iterator cbegin() const noexcept { return internal_instance_.begin(); }

  const_iterator cend() const noexcept { return internal_instance_.end(); }

  template <typename... _Args>
  std::pair<iterator, bool> emplace(_Args&&... __args) {
    return internal_instance_.Emplace(std::forward<_Args>(__args)...);
  }

  template <typename... _Args>
  iterator emplace_hint(const_iterator __pos, _Args&&... __args) {
    return internal_instance_.EmplaceHint(__pos,
                                          std::forward<_Args>(__args)...);
  }

  std::pair<iterator, bool> insert(const value_type& __x) {
    return internal_instance_.Insert(__x);
  }

  std::pair<iterator, bool> insert(value_type&& __x) {
    return internal_instance_.Insert(std::move(__x));
  }

  iterator insert(const_iterator __hint, const value_type& __x) {
    return internal_instance_.Insert(__hint, __x);
  }

  iterator insert(const_iterator __hint, value_type&& __x) {
    return internal_instance_.Insert(__hint, std::move(__x));
  }

  template <typename TInputIterator>
  void insert(TInputIterator __first, TInputIterator __last) {
    internal_instance_.Insert(__first, __last);
  }

  void insert(std::initializer_list<value_type> __l) {
    internal_instance_.Insert(__l);
  }

  node_type extract(const_iterator __pos) {
    return internal_instance_.UnsafeExtract(__pos);
  }

  node_type extract(const key_type& __key) {
    return internal_instance_.UnsafeExtract(__key);
  }

  iterator erase(const_iterator __position) {
    return internal_instance_.UnsafeErase(__position);
  }

  iterator erase(iterator __position) {
    return internal_instance_.UnsafeErase(__position);
  }

  size_type erase(const key_type& __x) {
    return internal_instance_.UnsafeErase(__x);
  }

  iterator erase(const_iterator __first, const_iterator __last) {
    return internal_instance_.UnsafeErase(__first, __last);
  }

  void clear() noexcept { internal_instance_.Clear(); }

  void swap(concurrent_unordered_set& __x) noexcept(
      noexcept(internal_instance_.Swap(__x.internal_instance_))) {
    internal_instance_.Swap(__x.internal_instance_);
  }

  template <typename TH2, typename TP2>
  void merge(concurrent_unordered_set<TKey, TH2, TP2, TAllocator>& __source) {
    internal_instance_.Merge(__source);
  }

  template <typename TH2, typename TP2>
  void merge(concurrent_unordered_set<TKey, TH2, TP2, TAllocator>&& __source) {
    merge(__source);
  }

  hasher hash_function() const { return internal_instance_.HashFunction(); }

  key_equal key_eq() const { return internal_instance_.KeyEq(); }

  iterator find(const key_type& __x) { return internal_instance_.Find(__x); }

  const_iterator find(const key_type& __x) const {
    return internal_instance_.Find(__x);
  }

  size_type count(const key_type& __x) const {
    return internal_instance_.Count(__x);
  }

  bool contains(const key_type& __x) const {
    return internal_instance_.Contains(__x);
  }

  std::pair<iterator, iterator> equal_range(const key_type& __x) {
    return internal_instance_.EqualRange(__x);
  }

  std::pair<const_iterator, const_iterator> equal_range(
      const key_type& __x) const {
    return internal_instance_.EqualRange(__x);
  }

  size_type bucket_count() const noexcept {
    return internal_instance_.UnsafeBucketCount();
  }

  size_type max_bucket_count() const noexcept {
    return internal_instance_.UnsafeMaxBucketCount();
  }

  size_type bucket_size(size_type __n) const {
    return internal_instance_.UnsafeBucketSize(__n);
  }

  size_type bucket(const key_type& __key) const {
    return internal_instance_.Bucket(__key);
  }

  local_iterator begin(size_type __n) { return internal_instance_.begin(__n); }

  const_local_iterator begin(size_type __n) const {
    return internal_instance_.begin(__n);
  }

  const_local_iterator cbegin(size_type __n) const {
    return internal_instance_.cbegin(__n);
  }

  local_iterator end(size_type __n) { return internal_instance_.end(__n); }

  const_local_iterator end(size_type __n) const {
    return internal_instance_.end(__n);
  }

  const_local_iterator cend(size_type __n) const {
    return internal_instance_.cend(__n);
  }

  float load_factor() const noexcept { return internal_instance_.LoadFactor(); }

  float max_load_factor() const noexcept {
    return internal_instance_.MaxLoadFactor();
  }

  void max_load_factor(float __z) { internal_instance_.MaxLoadFactor(__z); }

  void rehash(size_type __n) { internal_instance_.ReHash(__n); }

  void reserve(size_type __n) { internal_instance_.Reserve(__n); }
};

#if _CPP17_DEDUCTION_GUIDES_PRESENT

template <
    typename It,
    typename THash =
        std::hash<ws::concurrency::detail::templates::iterator_value_t<It>>,
    typename TKeyEq =
        std::equal_to<ws::concurrency::detail::templates::iterator_value_t<It>>,
    typename TAlloc = std::allocator<
        ws::concurrency::detail::templates::iterator_value_t<It>>,
    typename = std::enable_if_t<
        ws::concurrency::detail::containers::is_input_iterator_v<It>>,
    typename = std::enable_if_t<
        ws::concurrency::detail::templates::is_allocator_v<TAlloc>>,
    typename = std::enable_if_t<
        !ws::concurrency::detail::templates::is_allocator_v<THash>>,
    typename = std::enable_if_t<
        !ws::concurrency::detail::templates::is_allocator_v<TKeyEq>>,
    typename = std::enable_if_t<!std::is_integral_v<THash>>>
concurrent_unordered_set(It, It, std::size_t = {}, THash = THash(),
                         TKeyEq = TKeyEq(), TAlloc = TAlloc())
    -> concurrent_unordered_set<
        ws::concurrency::detail::templates::iterator_value_t<It>, THash, TKeyEq,
        TAlloc>;

template <typename T, typename THash = std::hash<T>,
          typename TKeyEq = std::equal_to<T>,
          typename TAlloc = std::allocator<T>,
          typename = std::enable_if_t<
              ws::concurrency::detail::templates::is_allocator_v<TAlloc>>,
          typename = std::enable_if_t<
              !ws::concurrency::detail::templates::is_allocator_v<THash>>,
          typename = std::enable_if_t<
              !ws::concurrency::detail::templates::is_allocator_v<TKeyEq>>,
          typename = std::enable_if_t<!std::is_integral_v<THash>>>
concurrent_unordered_set(std::initializer_list<T>, std::size_t = {},
                         THash = THash(), TKeyEq = TKeyEq(), TAlloc = TAlloc())
    -> concurrent_unordered_set<T, THash, TKeyEq, TAlloc>;

template <typename TIt, typename TAlloc,
          typename = std::enable_if_t<
              ws::concurrency::detail::containers::is_input_iterator_v<TIt>>,
          typename = std::enable_if_t<
              ws::concurrency::detail::templates::is_allocator_v<TAlloc>>>
concurrent_unordered_set(TIt, TIt, std::size_t, TAlloc)
    -> concurrent_unordered_set<
        ws::concurrency::detail::templates::iterator_value_t<TIt>,
        std::hash<ws::concurrency::detail::templates::iterator_value_t<TIt>>,
        std::equal_to<
            ws::concurrency::detail::templates::iterator_value_t<TIt>>,
        TAlloc>;

template <typename TIt, typename THash, typename Alloc,
          typename = std::enable_if_t<
              ws::concurrency::detail::containers::is_input_iterator_v<TIt>>,
          typename = std::enable_if_t<
              ws::concurrency::detail::templates::is_allocator_v<Alloc>>,
          typename = std::enable_if_t<
              !ws::concurrency::detail::templates::is_allocator_v<THash>>,
          typename = std::enable_if_t<!std::is_integral_v<THash>>>
concurrent_unordered_set(TIt, TIt, std::size_t, THash, Alloc)
    -> concurrent_unordered_set<
        ws::concurrency::detail::templates::iterator_value_t<TIt>, THash,
        std::equal_to<
            ws::concurrency::detail::templates::iterator_value_t<TIt>>,
        Alloc>;

template <typename T, typename TAlloc,
          typename = std::enable_if_t<
              ws::concurrency::detail::templates::is_allocator_v<TAlloc>>>
concurrent_unordered_set(std::initializer_list<T>, std::size_t, TAlloc)
    -> concurrent_unordered_set<T, std::hash<T>, std::equal_to<T>, TAlloc>;

template <typename T, typename TAlloc,
          typename = std::enable_if_t<
              ws::concurrency::detail::templates::is_allocator_v<TAlloc>>>
concurrent_unordered_set(std::initializer_list<T>, TAlloc)
    -> concurrent_unordered_set<T, std::hash<T>, std::equal_to<T>, TAlloc>;

template <typename T, typename Hash, typename TAlloc,
          typename = std::enable_if_t<
              ws::concurrency::detail::templates::is_allocator_v<TAlloc>>,
          typename = std::enable_if_t<
              !ws::concurrency::detail::templates::is_allocator_v<Hash>>,
          typename = std::enable_if_t<!std::is_integral_v<Hash>>>
concurrent_unordered_set(std::initializer_list<T>, std::size_t, Hash, TAlloc)
    -> concurrent_unordered_set<T, Hash, std::equal_to<T>, TAlloc>;

#endif

}  // namespace stl

}  // namespace collections
}  // namespace concurrency
}  // namespace ws
