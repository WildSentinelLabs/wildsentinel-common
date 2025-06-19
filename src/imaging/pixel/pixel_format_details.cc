#include "imaging/pixel/pixel_format_details.h"
namespace ws {
namespace imaging {

bool PixelFormatDetails::HasAlpha() const { return alpha_index != -1; }

}  // namespace imaging
}  // namespace ws
