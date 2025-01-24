#pragma once

#include "pixel/color_spaces/gray/gray.h"
#include "pixel/color_spaces/sycc/ycc.h"
#include "pixel/pixel_converter.h"

template <typename T>
class GrayToSYCCConverter : public PixelConverter {
 public:
  explicit GrayToSYCCConverter(uint8_t bit_depth);

  void Convert(const Gray<T>& gray, YCC<T>& ycc) const;
};
