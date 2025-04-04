#include "imaging/chroma_subsampling.h"
namespace ws {
namespace imaging {
const std::string ChromaSubsamplingToString(
    const ChromaSubsampling& subsampling) {
  switch (subsampling) {
    case ChromaSubsampling::kSamp444:
      return "SAMP444";
    case ChromaSubsampling::kSamp422:
      return "SAMP422";
    case ChromaSubsampling::kSamp420:
      return "SAMP420";
    case ChromaSubsampling::kSamp400:
      return "SAMP400";
    case ChromaSubsampling::kSamp440:
      return "SAMP440";
    case ChromaSubsampling::kSamp411:
      return "SAMP411";
    case ChromaSubsampling::kSamp441:
      return "SAMP441";
    default:
      return "Unsupported";
  }
}
}  // namespace imaging
}  // namespace ws
