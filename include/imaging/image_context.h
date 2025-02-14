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
  explicit ImageContext();

  explicit ImageContext(const std::unordered_map<std::string, int32_t> tags);

  ~ImageContext();

  void Add(const std::string key, const int32_t value);

  bool Contains(const std::string& key) const;

  std::optional<int32_t> Get(const std::string& key) const;

  std::unordered_map<std::string, int32_t>::iterator begin();

  std::unordered_map<std::string, int32_t>::iterator end();

  std::unordered_map<std::string, int32_t>::const_iterator begin() const;

  std::unordered_map<std::string, int32_t>::const_iterator end() const;

  std::string ToString() const;

  void Clean();

  void Dispose();

 private:
  std::unordered_map<std::string, int32_t> data;
};

std::ostream& operator<<(std::ostream& os, const ImageContext& context);

}  // namespace imaging
}  // namespace ws
