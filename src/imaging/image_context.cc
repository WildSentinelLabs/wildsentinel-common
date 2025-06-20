#include "imaging/image_context.h"
namespace ws {
namespace imaging {

ImageContext::ImageContext() : data(ImageContext::map_type()) {}

ImageContext::ImageContext(const ImageContext::map_type& tags) : data(tags) {}

bool ImageContext::Empty() const { return data.empty(); }

ImageContext::size_type ImageContext::Size() const { return data.size(); }

bool ImageContext::Contains(const ImageContext::key_type& key) const {
  return data.find(key) != data.end();
}

ImageContext::mapped_type& ImageContext::operator[](
    const ImageContext::key_type& key) {
  return data[key];
}

ImageContext::mapped_type& ImageContext::operator[](
    ImageContext::key_type&& key) {
  return data[std::move(key)];
}

std::optional<ImageContext::mapped_type> ImageContext::Get(
    const key_type& key) const {
  if (auto it = data.find(key); it != data.end()) {
    return it->second;
  }

  return std::nullopt;
}

void ImageContext::Add(const ImageContext::key_type& key,
                       ImageContext::mapped_type value) {
  auto it = data.find(key);
  if (it != data.end()) {
    it->second = value;
  } else {
    data.insert({key, value});
  }
}

ImageContext::map_type::iterator ImageContext::begin() { return data.begin(); }

ImageContext::map_type::iterator ImageContext::end() { return data.end(); }

ImageContext::map_type::const_iterator ImageContext::begin() const {
  return data.begin();
}

ImageContext::map_type::const_iterator ImageContext::end() const {
  return data.end();
}

std::string ImageContext::ToString() const {
  std::string result;
  for (const auto& [tag, value] : data) {
    result += Format("{}: {}\n", tag, value);
  }

  return result;
}

void ImageContext::Clear() { data.clear(); }

}  // namespace imaging
}  // namespace ws
