#pragma once

#include <cstdint>
#include <optional>
#include <string>

// #include "ws/concurrency/concurrent_unordered_map.h"
#include <unordered_map>

#include "ws/string/format.h"
#include "ws/string/transparent_hash.h"

namespace ws {
namespace imaging {
class ImageContext {
 public:
  using size_type = std::size_t;
  using key_type = std::string;
  using key_view_type = std::string_view;
  using mapped_type = std::int32_t;
  //   using map_type = std::concurrent_unordered_map<
  //   key_type, mapped_type, TransparentHash,
  //   TransparentEqual>;
  // FIXME: Use concurrent unordered map when available
  using map_type = std::unordered_map<key_type, mapped_type, TransparentHash,
                                      TransparentEqual>;

  ImageContext();
  ImageContext(const map_type& tags);
  ImageContext(
      std::initializer_list<std::pair<key_view_type, mapped_type>> tags);

  mapped_type& operator[](const key_type& key);
  mapped_type& operator[](key_type&& key);
  mapped_type& operator[](key_view_type key);

  ~ImageContext() = default;

  bool Empty() const;
  size_type Size() const;
  bool Contains(const key_type& key) const;
  std::optional<mapped_type> Get(const key_type& key) const;
  void Add(const key_type& key, mapped_type value);
  bool Contains(key_view_type key) const;
  std::optional<mapped_type> Get(key_view_type key) const;
  void Add(key_view_type key, mapped_type value);
  map_type::iterator begin();
  map_type::iterator end();
  map_type::const_iterator begin() const;
  map_type::const_iterator end() const;
  std::string ToString() const;
  void Clear();

 private:
  map_type data;
};
}  // namespace imaging
}  // namespace ws
