#pragma once

#include "imaging/color_spaces/color_space_converter.h"
#include "imaging/color_spaces/gray/gray.h"
#include "imaging/color_spaces/sycc/ycc.h"

template <typename T>
class GrayToSYCCConverter : public ColorSpaceConverter<T> {
 public:
  explicit GrayToSYCCConverter(uint8_t bit_depth);

  void Convert(const Gray<T>& gray, YCC<T>& ycc) const;
};
