#include "imaging/pixel_format_details.h"

bool PixelFormatDetails::has_alpha() const { return alpha_index != -1; }
