#pragma once

#include "imaging/color_spaces/color_space_converter.h"
#include "imaging/color_spaces/gray/gray.h"
#include "imaging/color_spaces/sycc/ycc.h"

template <typename T>
class SYCCToGrayConverter : public ColorSpaceConverter<T> {
 public:
  explicit SYCCToGrayConverter(uint8_t bit_depth);

  void Convert(const YCC<T>& sycc, Gray<T>& gray) const;
};
