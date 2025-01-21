#pragma once
#include <iostream>
#include <map>
#include <optional>
#include <sstream>
#include <string>

struct ImageContext {
 public:
  explicit ImageContext();

  explicit ImageContext(const std::map<std::string, int32_t> tags);

  ~ImageContext();

  void Add(const std::string key, const int32_t value);

  bool Contains(const std::string& key) const;

  std::optional<int32_t> Get(const std::string& key) const;

  std::string ToString() const;

  void Dispose();

 private:
  std::map<std::string, int32_t> data;
};

std::ostream& operator<<(std::ostream& os, const ImageContext& context);
