#pragma once

#include "pixel/color_spaces/gray/gray.h"
#include "pixel/color_spaces/sycc/ycc.h"
#include "pixel/pixel_converter.h"

template <typename T>
class SYCCToGrayConverter : public PixelConverter {
 public:
  explicit SYCCToGrayConverter(uint8_t bit_depth);

  void Convert(const YCC<T>& sycc, Gray<T>& gray) const;
};
