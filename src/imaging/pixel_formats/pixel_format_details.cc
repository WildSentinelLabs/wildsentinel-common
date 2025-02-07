#include "imaging/pixel_formats/pixel_format_details.h"

bool PixelFormatDetails::HasAlpha() const { return alpha_index != -1; }
