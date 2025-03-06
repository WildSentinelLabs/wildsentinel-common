#pragma once
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <unordered_map>
namespace ws {
namespace imaging {

struct ImageContext {
 public:
  using key_type = std::string;
  using mapped_type = std::int32_t;
  using size_type = std::size_t;
  using map_type = std::unordered_map<key_type, mapped_type>;

  explicit ImageContext();

  explicit ImageContext(const map_type& tags);

  ~ImageContext();

  bool Empty() const;

  size_type Size() const;

  bool Contains(const key_type& key) const;

  mapped_type& operator[](const key_type& key);

  mapped_type& operator[](key_type&& key);

  std::optional<mapped_type> Get(const key_type& key) const;

  void Add(const key_type key, const mapped_type value);

  map_type::iterator begin();

  map_type::iterator end();

  map_type::const_iterator begin() const;

  map_type::const_iterator end() const;

  std::string ToString() const;

  void Clear();

  void Dispose();

 private:
  map_type data;
};

std::ostream& operator<<(std::ostream& os, const ImageContext& context);

}  // namespace imaging
}  // namespace ws
