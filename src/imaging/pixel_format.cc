#include "imaging/pixel_format.h"

PixelFormat::PixelFormat(const ColorSpace color_space,
                         const uint8_t num_components,
                         const ChromaSubsampling subsampling)
    : color_space(color_space),
      num_components(num_components),
      subsampling(subsampling) {
  if (!IsValid()) {
    throw std::invalid_argument(
        "Parameters must be supported, non-null and greater than zero");
  }
};

PixelFormat::PixelFormat(const ColorSpace color_space,
                         const uint8_t num_components)
    : color_space(color_space),
      num_components(num_components),
      subsampling(ChromaSubsampling::kSAMP_444) {
  if (!IsValid()) {
    throw std::invalid_argument(
        "Parameters must be supported, non-null and greater than zero");
  }
};

bool PixelFormat::IsValid() const {
  return color_space != ColorSpace::kUNSUPPORTED && num_components != 0 &&
         subsampling != ChromaSubsampling::kUNSUPPORTED;
}

std::string PixelFormat::ToString() const {
  std::stringstream ss;
  ss << "PixelFormat("
     << "color_space: " << ColorSpaceToString(color_space) << ", "
     << "num_components: " << static_cast<int>(num_components) << ", "
     << "subsampling: " << ChromaSubsamplingToString(subsampling) << ")";
  return ss.str();
}

bool PixelFormat::operator==(const PixelFormat& other) const {
  return color_space == other.color_space &&
         num_components == other.num_components &&
         subsampling == other.subsampling;
}

bool PixelFormat::operator!=(const PixelFormat& other) const {
  return !(*this == other);
}

std::ostream& operator<<(std::ostream& os, const PixelFormat& format) {
  os << format.ToString();
  return os;
}
