#include "imaging/chroma_subsampling.h"

const std::string ChromaSubsamplingToString(
    const ChromaSubsampling& subsampling) {
  switch (subsampling) {
    case ChromaSubsampling::kSAMP_444:
      return "SAMP_444";
    case ChromaSubsampling::kSAMP_422:
      return "SAMP_422";
    case ChromaSubsampling::kSAMP_420:
      return "SAMP_420";
    case ChromaSubsampling::kSAMP_400:
      return "SAMP_400";
    case ChromaSubsampling::kSAMP_440:
      return "SAMP_440";
    case ChromaSubsampling::kSAMP_411:
      return "SAMP_411";
    case ChromaSubsampling::kSAMP_441:
      return "SAMP_441";
    default:
      return "UNSUPPORTED";
  }
}

std::ostream& operator<<(std::ostream& os,
                         const ChromaSubsampling& subsampling) {
  os << ChromaSubsamplingToString(subsampling);
  return os;
}
