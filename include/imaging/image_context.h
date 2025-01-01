#pragma once
#include <iostream>
#include <map>
#include <optional>
#include <sstream>
#include <string>

#include "imaging/image_defs.h"

struct ImageContext {
 public:
  explicit ImageContext(const ImageFormat& format);

  explicit ImageContext(const std::map<ImageTag, long>& tags,
                        const ImageFormat& format);

  ~ImageContext();

  void Add(const ImageTag& key, const long& value);

  bool Contains(const ImageTag& key) const;

  std::optional<long> Get(const ImageTag& key) const;

  ImageFormat GetImageFormat() const;

  std::string ToString() const;

  void Dispose();

 private:
  std::map<ImageTag, long> data;
  ImageFormat format;
};

std::ostream& operator<<(std::ostream& os, const ImageContext& context);
