// Based on oneTBB (https://github.com/uxlfoundation/oneTBB)
// See THIRD-PARTY-NOTICES

#pragma once

#include "ws/concurrency/internal/concurrent_unordered_base.h"
#include "ws/concurrency/internal/helpers.h"

namespace ws {
namespace concurrency {
template <typename TKey, typename T, typename THash, typename TKeyEqual,
          typename TAllocator, bool AllowMultimapping>
struct ConcurrentUnorderedMapTraits {
  using value_type = std::pair<const TKey, T>;
  using key_type = TKey;
  using allocator_type = TAllocator;
  using hash_compare_type =
      ws::concurrency::internal::HashCompare<TKey, THash, TKeyEqual>;
  static constexpr bool kAllowMultimapping = AllowMultimapping;

  static constexpr const key_type& GetKey(const value_type& value) {
    return value.first;
  }
};

template <typename TKey, typename T, typename THash = std::hash<TKey>,
          typename TKeyEqual = std::equal_to<TKey>,
          typename TAllocator = std::allocator<std::pair<const TKey, T>>>
class ConcurrentUnorderedMap
    : public ws::concurrency::internal::ConcurrentUnorderedBase<
          ConcurrentUnorderedMapTraits<TKey, T, THash, TKeyEqual, TAllocator,
                                       false>> {
  using traits_type = ConcurrentUnorderedMapTraits<TKey, T, THash, TKeyEqual,
                                                   TAllocator, false>;
  using base_type =
      ws::concurrency::internal::ConcurrentUnorderedBase<traits_type>;

 public:
  using key_type = typename base_type::key_type;
  using mapped_type = T;
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

  ConcurrentUnorderedMap() = default;
  ConcurrentUnorderedMap(const ConcurrentUnorderedMap&) = default;
  ConcurrentUnorderedMap(const ConcurrentUnorderedMap& other,
                         const allocator_type& alloc)
      : base_type(other, alloc) {}
  ConcurrentUnorderedMap(ConcurrentUnorderedMap&&) = default;
  ConcurrentUnorderedMap(ConcurrentUnorderedMap&& other,
                         const allocator_type& alloc)
      : base_type(std::move(other), alloc) {}

  ConcurrentUnorderedMap& operator=(const ConcurrentUnorderedMap&) = default;
  ConcurrentUnorderedMap& operator=(ConcurrentUnorderedMap&&) = default;

  ConcurrentUnorderedMap& operator=(std::initializer_list<value_type> il) {
    base_type::operator=(il);
    return *this;
  }

  mapped_type& operator[](const key_type& key) {
    iterator where = this->Find(key);

    if (where == this->end()) {
      where = this->Emplace(std::piecewise_construct,
                            std::forward_as_tuple(key), std::tuple<>())
                  .first;
    }
    return where->second;
  }

  mapped_type& operator[](key_type&& key) {
    iterator where = this->Find(key);

    if (where == this->end()) {
      where =
          this->Emplace(std::piecewise_construct,
                        std::forward_as_tuple(std::move(key)), std::tuple<>())
              .first;
    }

    return where->second;
  }

  mapped_type& At(const key_type& key) {
    iterator where = this->Find(key);

    if (where == this->end())
      throw std::invalid_argument("ConcurrentUnorderedMap::at");

    return where->second;
  }

  const mapped_type& At(const key_type& key) const {
    const_iterator where = this->Find(key);

    if (where == this->end())
      throw std::out_of_range("ConcurrentUnorderedMap::at");

    return where->second;
  }

  using base_type::Insert;

  template <typename TP>
  typename std::enable_if<std::is_constructible<value_type, TP&&>::value,
                          std::pair<iterator, bool>>::type
  Insert(TP&& value) {
    return this->Emplace(std::forward<TP>(value));
  }

  template <typename TP>
  typename std::enable_if<std::is_constructible<value_type, TP&&>::value,
                          iterator>::type
  Insert(const_iterator hint, TP&& value) {
    return this->EmplaceHint(hint, std::forward<TP>(value));
  }

  template <typename TOtherHash, typename TOtherKeyEqual>
  void Merge(ConcurrentUnorderedMap<key_type, mapped_type, TOtherHash,
                                    TOtherKeyEqual, allocator_type>& source) {
    this->InternalMerge(source);
  }

  template <typename TOtherHash, typename TOtherKeyEqual>
  void Merge(ConcurrentUnorderedMap<key_type, mapped_type, TOtherHash,
                                    TOtherKeyEqual, allocator_type>&& source) {
    this->InternalMerge(std::move(source));
  }
};

#if _CPP17_DEDUCTION_GUIDES_PRESENT
template <typename TIt,
          typename THash = std::hash<
              ws::concurrency::internal::templates::iterator_key_t<TIt>>,
          typename TKeyEq = std::equal_to<
              ws::concurrency::internal::templates::iterator_key_t<TIt>>,
          typename TAlloc = std::allocator<
              ws::concurrency::internal::templates::iterator_alloc_pair_t<TIt>>,
          typename = std::enable_if_t<
              ws::concurrency::internal::containers::is_input_iterator_v<TIt>>,
          typename = std::enable_if_t<
              ws::concurrency::internal::templates::is_allocator_v<TAlloc>>,
          typename = std::enable_if_t<
              !ws::concurrency::internal::templates::is_allocator_v<THash>>,
          typename = std::enable_if_t<
              !ws::concurrency::internal::templates::is_allocator_v<TKeyEq>>,
          typename = std::enable_if_t<!std::is_integral_v<THash>>>
ConcurrentUnorderedMap(TIt, TIt, std::size_t = {}, THash = THash(),
                       TKeyEq = TKeyEq(), TAlloc = TAlloc())
    -> ConcurrentUnorderedMap<
        ws::concurrency::internal::templates::iterator_key_t<TIt>,
        ws::concurrency::internal::templates::iterator_mapped_t<TIt>, THash,
        TKeyEq, TAlloc>;

template <typename TKey, typename T,
          typename THash = std::hash<std::remove_const_t<TKey>>,
          typename KeyEq = std::equal_to<std::remove_const_t<TKey>>,
          typename Alloc = std::allocator<std::pair<const TKey, T>>,
          typename = std::enable_if_t<
              ws::concurrency::internal::templates::is_allocator_v<Alloc>>,
          typename = std::enable_if_t<
              !ws::concurrency::internal::templates::is_allocator_v<THash>>,
          typename = std::enable_if_t<
              !ws::concurrency::internal::templates::is_allocator_v<KeyEq>>,
          typename = std::enable_if_t<!std::is_integral_v<THash>>>
ConcurrentUnorderedMap(std::initializer_list<std::pair<TKey, T>>,
                       std::size_t = {}, THash = THash(), KeyEq = KeyEq(),
                       Alloc = Alloc())
    -> ConcurrentUnorderedMap<std::remove_const_t<TKey>, T, THash, KeyEq,
                              Alloc>;

template <typename TIt, typename TAlloc,
          typename = std::enable_if_t<
              ws::concurrency::internal::containers::is_input_iterator_v<TIt>>,
          typename = std::enable_if_t<
              ws::concurrency::internal::templates::is_allocator_v<TAlloc>>>
ConcurrentUnorderedMap(TIt, TIt, std::size_t, TAlloc) -> ConcurrentUnorderedMap<
    ws::concurrency::internal::templates::iterator_key_t<TIt>,
    ws::concurrency::internal::templates::iterator_mapped_t<TIt>,
    std::hash<ws::concurrency::internal::templates::iterator_key_t<TIt>>,
    std::equal_to<ws::concurrency::internal::templates::iterator_key_t<TIt>>,
    TAlloc>;

template <typename TIt, typename THash, typename TAlloc,
          typename = std::enable_if_t<
              ws::concurrency::internal::containers::is_input_iterator_v<TIt>>,
          typename = std::enable_if_t<
              ws::concurrency::internal::templates::is_allocator_v<TAlloc>>,
          typename = std::enable_if_t<
              !ws::concurrency::internal::templates::is_allocator_v<THash>>,
          typename = std::enable_if_t<!std::is_integral_v<THash>>>
ConcurrentUnorderedMap(TIt, TIt, std::size_t, THash, TAlloc)
    -> ConcurrentUnorderedMap<
        ws::concurrency::internal::templates::iterator_key_t<TIt>,
        ws::concurrency::internal::templates::iterator_mapped_t<TIt>, THash,
        std::equal_to<
            ws::concurrency::internal::templates::iterator_key_t<TIt>>,
        TAlloc>;

template <typename TKey, typename T, typename TAlloc,
          typename = std::enable_if_t<
              ws::concurrency::internal::templates::is_allocator_v<TAlloc>>>
ConcurrentUnorderedMap(std::initializer_list<std::pair<TKey, T>>, std::size_t,
                       TAlloc)
    -> ConcurrentUnorderedMap<std::remove_const_t<TKey>, T,
                              std::hash<std::remove_const_t<TKey>>,
                              std::equal_to<std::remove_const_t<TKey>>, TAlloc>;

template <typename TKey, typename T, typename TAlloc,
          typename = std::enable_if_t<
              ws::concurrency::internal::templates::is_allocator_v<TAlloc>>>
ConcurrentUnorderedMap(std::initializer_list<std::pair<TKey, T>>, TAlloc)
    -> ConcurrentUnorderedMap<std::remove_const_t<TKey>, T,
                              std::hash<std::remove_const_t<TKey>>,
                              std::equal_to<std::remove_const_t<TKey>>, TAlloc>;

template <typename TKey, typename T, typename THash, typename TAlloc,
          typename = std::enable_if_t<
              ws::concurrency::internal::templates::is_allocator_v<TAlloc>>,
          typename = std::enable_if_t<
              !ws::concurrency::internal::templates::is_allocator_v<THash>>,
          typename = std::enable_if_t<!std::is_integral_v<THash>>>
ConcurrentUnorderedMap(std::initializer_list<std::pair<TKey, T>>, std::size_t,
                       THash, TAlloc)
    -> ConcurrentUnorderedMap<std::remove_const_t<TKey>, T, THash,
                              std::equal_to<std::remove_const_t<TKey>>, TAlloc>;

#endif

template <typename TKey, typename T, typename THash, typename TKeyEqual,
          typename TAllocator>
void Swap(ConcurrentUnorderedMap<TKey, T, THash, TKeyEqual, TAllocator>& lhs,
          ConcurrentUnorderedMap<TKey, T, THash, TKeyEqual, TAllocator>& rhs) {
  lhs.Swap(rhs);
}

namespace stl {

template <typename TKey, typename T, typename THash = std::hash<TKey>,
          typename TKeyEqual = std::equal_to<TKey>,
          typename TAllocator = std::allocator<std::pair<const TKey, T>>>
class concurrent_unordered_map {
 public:
  using hashtable_type =
      ConcurrentUnorderedMap<TKey, T, THash, TKeyEqual, TAllocator>;
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
  concurrent_unordered_map() = default;

  concurrent_unordered_map(const concurrent_unordered_map& other)
      : internal_instance_(other.internal_instance_) {}

  concurrent_unordered_map(concurrent_unordered_map&& other) noexcept
      : internal_instance_(std::move(other.internal_instance_)) {}

  concurrent_unordered_map(const allocator_type& alloc)
      : internal_instance_(alloc) {}

  concurrent_unordered_map(const concurrent_unordered_map& other,
                           const allocator_type& alloc)
      : internal_instance_(other.internal_instance_, alloc) {}

  concurrent_unordered_map(concurrent_unordered_map&& other,
                           const allocator_type& alloc) noexcept
      : internal_instance_(std::move(other.internal_instance_), alloc) {}

  concurrent_unordered_map(std::initializer_list<value_type> il,
                           size_type n = 0, const hasher& hf = hasher(),
                           const key_equal& eql = key_equal(),
                           const allocator_type& a = allocator_type())
      : internal_instance_(il, n, hf, eql, a) {}

  concurrent_unordered_map(size_type n, const allocator_type& a)
      : internal_instance_(n, a) {}

  concurrent_unordered_map(size_type n, const hasher& hf,
                           const allocator_type& a)
      : internal_instance_(n, hf, a) {}

  template <typename InputIt>
  concurrent_unordered_map(InputIt first, InputIt last, size_type n,
                           const allocator_type& a)
      : internal_instance_(first, last, n, a) {}

  template <typename InputIt>
  concurrent_unordered_map(InputIt first, InputIt last, size_type n,
                           const hasher& hf, const allocator_type& a)
      : internal_instance_(first, last, n, hf, a) {}

  concurrent_unordered_map(std::initializer_list<value_type> il, size_type n,
                           const allocator_type& a)
      : internal_instance_(il, n, a) {}

  concurrent_unordered_map(std::initializer_list<value_type> il, size_type n,
                           const hasher& hf, const allocator_type& a)
      : internal_instance_(il, n, hf, a) {}

  concurrent_unordered_map& operator=(const concurrent_unordered_map& other) {
    internal_instance_ = other.internal_instance_;
    return *this;
  }

  concurrent_unordered_map& operator=(
      concurrent_unordered_map&& other) noexcept {
    internal_instance_ = std::move(other.internal_instance_);
    return *this;
  }

  concurrent_unordered_map& operator=(std::initializer_list<value_type> il) {
    internal_instance_ = il;
    return *this;
  }

  allocator_type get_allocator() const noexcept {
    return internal_instance_.get_allocator();
  }

  bool empty() const noexcept { return internal_instance_.Empty(); }

  size_type size() const noexcept { return internal_instance_.Size(); }

  size_type max_size() const noexcept { return internal_instance_.MaxSize(); }

  iterator begin() noexcept { return internal_instance_.begin(); }

  const_iterator begin() const noexcept { return internal_instance_.begin(); }

  const_iterator cbegin() const noexcept { return internal_instance_.cbegin(); }

  iterator end() noexcept { return internal_instance_.end(); }

  const_iterator end() const noexcept { return internal_instance_.end(); }

  const_iterator cend() const noexcept { return internal_instance_.cend(); }

  template <typename... TArgs>
  std::pair<iterator, bool> emplace(TArgs&&... __args) {
    return internal_instance_.Emplace(std::forward<TArgs>(__args)...);
  }

  template <typename... TArgs>
  iterator emplace_hint(const_iterator __pos, TArgs&&... __args) {
    return internal_instance_.EmplaceHint(__pos,
                                          std::forward<TArgs>(__args)...);
  }

  node_type extract(const_iterator __pos) {
    return internal_instance_.UnsafeExtract(__pos);
  }

  node_type extract(const key_type& __key) {
    return internal_instance_.UnsafeExtract(__key);
  }

  iterator insert(const_iterator, node_type&& __nh) {
    return internal_instance_.Insert(std::move(__nh)).position;
  }

  std::pair<iterator, bool> insert(const value_type& __x) {
    return internal_instance_.Insert(__x);
  }

  std::pair<iterator, bool> insert(value_type&& __x) {
    return internal_instance_.Insert(std::move(__x));
  }

  template <typename TPair>
  std::enable_if<std::is_constructible<value_type, TPair&&>::value,
                 std::pair<iterator, bool>>
  insert(TPair&& __x) {
    return internal_instance_.Emplace(std::forward<TPair>(__x));
  }

  iterator insert(const_iterator __hint, const value_type& __x) {
    return internal_instance_.Insert(__hint, __x);
  }

  iterator insert(const_iterator __hint, value_type&& __x) {
    return internal_instance_.Insert(__hint, std::move(__x));
  }

  template <typename TPair>
  std::enable_if<std::is_constructible<value_type, TPair&&>::value, iterator>
  insert(const_iterator __hint, TPair&& __x) {
    return internal_instance_.EmplaceHint(__hint, std::forward<TPair>(__x));
  }

  template <typename TInputIterator>
  void insert(TInputIterator __first, TInputIterator __last) {
    internal_instance_.Insert(__first, __last);
  }

  void insert(std::initializer_list<value_type> __l) {
    internal_instance_.Insert(__l);
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

  void swap(concurrent_unordered_map& __x) noexcept(
      noexcept(internal_instance_.Swap(__x.internal_instance_))) {
    internal_instance_.Swap(__x.internal_instance_);
  }

  template <typename TH2, typename TP2>
  void merge(
      concurrent_unordered_map<TKey, T, TH2, TP2, TAllocator>& __source) {
    internal_instance_.Merge(__source.internal_instance_);
  }

  template <typename TH2, typename TP2>
  void merge(
      concurrent_unordered_map<TKey, T, TH2, TP2, TAllocator>&& __source) {
    internal_instance_.Merge(__source.internal_instance_);
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

  mapped_type& operator[](const key_type& __k) {
    return internal_instance_[__k];
  }

  mapped_type& operator[](key_type&& __k) {
    return internal_instance_[std::move(__k)];
  }

  mapped_type& at(const key_type& __k) { return internal_instance_.At(__k); }

  const mapped_type& at(const key_type& __k) const {
    return internal_instance_.At(__k);
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
    return internal_instance_.UnsafeBucket(__key);
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

  template <typename TKey1, typename T1, typename THash1, typename TPred1,
            typename TAllocator1>
  friend bool operator==(
      const concurrent_unordered_map<TKey1, T1, THash1, TPred1, TAllocator1>&
          lhs,
      const concurrent_unordered_map<TKey1, T1, THash1, TPred1, TAllocator1>&
          rhs);
};

template <typename TKey1, typename T1, typename THash1, typename TPred1,
          typename TAllocator1>
inline bool operator==(
    const concurrent_unordered_map<TKey1, T1, THash1, TPred1, TAllocator1>& lhs,
    const concurrent_unordered_map<TKey1, T1, THash1, TPred1, TAllocator1>&
        rhs) {
  if (&lhs == &rhs) {
    return true;
  }

  return lhs.internal_instance_ == rhs.internal_instance_;
}

#if _CPP17_DEDUCTION_GUIDES_PRESENT
template <typename TIt,
          typename Hash = std::hash<
              ws::concurrency::internal::templates::iterator_key_t<TIt>>,
          typename KeyEq = std::equal_to<
              ws::concurrency::internal::templates::iterator_key_t<TIt>>,
          typename Alloc = std::allocator<
              ws::concurrency::internal::templates::iterator_alloc_pair_t<TIt>>,
          typename = std::enable_if_t<
              ws::concurrency::internal::containers::is_input_iterator_v<TIt>>,
          typename = std::enable_if_t<
              ws::concurrency::internal::templates::is_allocator_v<Alloc>>,
          typename = std::enable_if_t<
              !ws::concurrency::internal::templates::is_allocator_v<Hash>>,
          typename = std::enable_if_t<
              !ws::concurrency::internal::templates::is_allocator_v<KeyEq>>,
          typename = std::enable_if_t<!std::is_integral_v<Hash>>>
concurrent_unordered_map(TIt, TIt, std::size_t = {}, Hash = Hash(),
                         KeyEq = KeyEq(), Alloc = Alloc())
    -> concurrent_unordered_map<
        ws::concurrency::internal::templates::iterator_key_t<TIt>,
        ws::concurrency::internal::templates::iterator_mapped_t<TIt>, Hash,
        KeyEq, Alloc>;

template <typename TKey, typename T,
          typename Hash = std::hash<std::remove_const_t<TKey>>,
          typename KeyEq = std::equal_to<std::remove_const_t<TKey>>,
          typename Alloc = std::allocator<std::pair<const TKey, T>>,
          typename = std::enable_if_t<
              ws::concurrency::internal::templates::is_allocator_v<Alloc>>,
          typename = std::enable_if_t<
              !ws::concurrency::internal::templates::is_allocator_v<Hash>>,
          typename = std::enable_if_t<
              !ws::concurrency::internal::templates::is_allocator_v<KeyEq>>,
          typename = std::enable_if_t<!std::is_integral_v<Hash>>>
concurrent_unordered_map(std::initializer_list<std::pair<TKey, T>>,
                         std::size_t = {}, Hash = Hash(), KeyEq = KeyEq(),
                         Alloc = Alloc())
    -> concurrent_unordered_map<std::remove_const_t<TKey>, T, Hash, KeyEq,
                                Alloc>;

template <typename TIt, typename Alloc,
          typename = std::enable_if_t<
              ws::concurrency::internal::containers::is_input_iterator_v<TIt>>,
          typename = std::enable_if_t<
              ws::concurrency::internal::templates::is_allocator_v<Alloc>>>
concurrent_unordered_map(TIt, TIt, std::size_t, Alloc)
    -> concurrent_unordered_map<
        ws::concurrency::internal::templates::iterator_key_t<TIt>,
        ws::concurrency::internal::templates::iterator_mapped_t<TIt>,
        std::hash<ws::concurrency::internal::templates::iterator_key_t<TIt>>,
        std::equal_to<
            ws::concurrency::internal::templates::iterator_key_t<TIt>>,
        Alloc>;

template <typename TIt, typename Hash, typename Alloc,
          typename = std::enable_if_t<
              ws::concurrency::internal::containers::is_input_iterator_v<TIt>>,
          typename = std::enable_if_t<
              ws::concurrency::internal::templates::is_allocator_v<Alloc>>,
          typename = std::enable_if_t<
              !ws::concurrency::internal::templates::is_allocator_v<Hash>>,
          typename = std::enable_if_t<!std::is_integral_v<Hash>>>
concurrent_unordered_map(TIt, TIt, std::size_t, Hash, Alloc)
    -> concurrent_unordered_map<
        ws::concurrency::internal::templates::iterator_key_t<TIt>,
        ws::concurrency::internal::templates::iterator_mapped_t<TIt>, Hash,
        std::equal_to<
            ws::concurrency::internal::templates::iterator_key_t<TIt>>,
        Alloc>;

template <typename TKey, typename T, typename Alloc,
          typename = std::enable_if_t<
              ws::concurrency::internal::templates::is_allocator_v<Alloc>>>
concurrent_unordered_map(std::initializer_list<std::pair<TKey, T>>, std::size_t,
                         Alloc)
    -> concurrent_unordered_map<
        std::remove_const_t<TKey>, T, std::hash<std::remove_const_t<TKey>>,
        std::equal_to<std::remove_const_t<TKey>>, Alloc>;

template <typename TKey, typename T, typename Alloc,
          typename = std::enable_if_t<
              ws::concurrency::internal::templates::is_allocator_v<Alloc>>>
concurrent_unordered_map(std::initializer_list<std::pair<TKey, T>>, Alloc)
    -> concurrent_unordered_map<
        std::remove_const_t<TKey>, T, std::hash<std::remove_const_t<TKey>>,
        std::equal_to<std::remove_const_t<TKey>>, Alloc>;

template <typename TKey, typename T, typename Hash, typename Alloc,
          typename = std::enable_if_t<
              ws::concurrency::internal::templates::is_allocator_v<Alloc>>,
          typename = std::enable_if_t<
              !ws::concurrency::internal::templates::is_allocator_v<Hash>>,
          typename = std::enable_if_t<!std::is_integral_v<Hash>>>
concurrent_unordered_map(std::initializer_list<std::pair<TKey, T>>, std::size_t,
                         Hash, Alloc)
    -> concurrent_unordered_map<std::remove_const_t<TKey>, T, Hash,
                                std::equal_to<std::remove_const_t<TKey>>,
                                Alloc>;

#endif
}  // namespace stl
}  // namespace concurrency
}  // namespace ws
