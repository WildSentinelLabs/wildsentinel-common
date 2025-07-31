#include "ws/imaging/image_context.h"
namespace ws {
namespace imaging {

ImageContext::ImageContext() : data(ImageContext::map_type()) {}

ImageContext::ImageContext(const ImageContext::map_type& tags) : data(tags) {}

ImageContext::ImageContext(
    std::initializer_list<std::pair<key_view_type, mapped_type>> tags)
    : ImageContext() {
  for (const auto& tag : tags) {
    data.emplace(tag.first, tag.second);
  }
}

ImageContext::mapped_type& ImageContext::operator[](
    const ImageContext::key_type& key) {
  return data[key];
}

ImageContext::mapped_type& ImageContext::operator[](
    ImageContext::key_type&& key) {
  return data[std::move(key)];
}

ImageContext::mapped_type& ImageContext::operator[](
    ImageContext::key_view_type key) {
  return data[key_type(key)];
}

bool ImageContext::Empty() const { return data.empty(); }

ImageContext::size_type ImageContext::Size() const { return data.size(); }

bool ImageContext::Contains(const ImageContext::key_type& key) const {
  return data.find(key) != data.end();
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

bool ImageContext::Contains(ImageContext::key_view_type key) const {
  return data.find(key) != data.end();
}

std::optional<ImageContext::mapped_type> ImageContext::Get(
    ImageContext::key_view_type key) const {
  if (auto it = data.find(key); it != data.end()) {
    return it->second;
  }
  return std::nullopt;
}

void ImageContext::Add(ImageContext::key_view_type key,
                       ImageContext::mapped_type value) {
  auto it = data.find(key);
  if (it != data.end()) {
    it->second = value;
  } else {
    data.insert({key_type(key), value});
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
  std::string result = Format("ImageContext[{}](", data.size());
  if (data.empty()) {
    result += ")";
    return result;
  }

  bool first = true;
  for (const auto& [tag, value] : data) {
    if (!first) {
      result += ", ";
    }

    result += Format("{{{}: {}}}", tag, value);
    first = false;
  }

  result += ")";
  return result;
}

void ImageContext::Clear() { data.clear(); }

}  // namespace imaging
}  // namespace ws
