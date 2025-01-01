#include "imaging/image_context.h"

ImageContext::ImageContext(const ImageFormat& format) : format(format) {}

ImageContext::ImageContext(const std::map<ImageTag, long>& tags,
                           const ImageFormat& format)
    : data(tags), format(format) {}

ImageContext::~ImageContext() { Dispose(); }

void ImageContext::Add(const ImageTag& key, const long& value) {
  data.insert_or_assign(key, value);
}

bool ImageContext::Contains(const ImageTag& key) const {
  return data.find(key) != data.end();
}

std::optional<long> ImageContext::Get(const ImageTag& key) const {
  std::map<ImageTag, long>::const_iterator it = data.find(key);
  if (it != data.end()) {
    return it->second;
  }
  return std::nullopt;
}

ImageFormat ImageContext::GetImageFormat() const { return format; }

std::string ImageContext::ToString() const {
  std::ostringstream ss;
  for (const auto& [tag, value] : data) {
    const std::string tagStr = ImageTagToString(tag);
    ss << tagStr << ": " << value << "\n";
  }
  return ss.str();
}

void ImageContext::Dispose() {
  if (data.size() > 0) data.clear();
}

std::ostream& operator<<(std::ostream& os, const ImageContext& context) {
  std::string str = context.ToString();
  os << str;
  return os;
}
