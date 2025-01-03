#pragma once
#include <iostream>
#include <map>
#include <optional>
#include <sstream>
#include <string>

#include "imaging/image_defs.h"

struct ImageContext {
 public:
  explicit ImageContext();

  explicit ImageContext(const std::map<ImageTag, long> tags);

  ~ImageContext();

  void Add(const ImageTag key, const long value);

  bool Contains(const ImageTag& key) const;

  std::optional<long> Get(const ImageTag& key) const;

  std::string ToString() const;

  void Dispose();

 private:
  std::map<ImageTag, long> data;
};

std::ostream& operator<<(std::ostream& os, const ImageContext& context);
