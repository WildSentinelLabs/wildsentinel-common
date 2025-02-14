#include "imaging/pixel/pixel_format_details.h"
namespace ws {
namespace imaging {
namespace pixel {
bool PixelFormatDetails::HasAlpha() const { return alpha_index != -1; }
}  // namespace pixel
}  // namespace imaging
}  // namespace ws
