#include "imaging/image_context.h"

ImageContext::ImageContext() : data(std::map<std::string, int32_t>()) {}

ImageContext::ImageContext(const std::map<std::string, int32_t> tags)
    : data(tags) {}

ImageContext::~ImageContext() { Dispose(); }

void ImageContext::Add(const std::string key, const int32_t value) {
  data.insert_or_assign(key, value);
}

bool ImageContext::Contains(const std::string& key) const {
  return data.find(key) != data.end();
}

std::optional<int32_t> ImageContext::Get(const std::string& key) const {
  std::map<std::string, int32_t>::const_iterator it = data.find(key);
  if (it != data.end()) {
    return it->second;
  }
  return std::nullopt;
}

std::map<std::string, int32_t>::iterator ImageContext::begin() {
  return data.begin();
}

std::map<std::string, int32_t>::iterator ImageContext::end() {
  return data.end();
}

std::map<std::string, int32_t>::const_iterator ImageContext::begin() const {
  return data.begin();
}

std::map<std::string, int32_t>::const_iterator ImageContext::end() const {
  return data.end();
}

std::string ImageContext::ToString() const {
  std::ostringstream ss;
  for (const auto& [tag, value] : data) {
    ss << tag << ": " << value << "\n";
  }

  return ss.str();
}

void ImageContext::Clean() {
  if (data.size() > 0) data.clear();
}

void ImageContext::Dispose() { Clean(); }

std::ostream& operator<<(std::ostream& os, const ImageContext& context) {
  std::string str = context.ToString();
  os << str;
  return os;
}
