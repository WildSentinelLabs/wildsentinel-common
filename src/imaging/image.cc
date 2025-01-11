#include "imaging/image.h"

Image::Image(ImageCore* core) : core_(core), context_(new ImageContext()) {
  if (!IsValid()) throw std::runtime_error("Image is not valid");
}

Image::Image(ImageCore* core, ImageContext* context)
    : core_(core), context_(context) {
  if (!IsValid()) throw std::runtime_error("Image is not valid");
}

Image::~Image() { Dispose(); }

const ImageCore& Image::GetCore() const { return *core_; }

const ImageContext& Image::GetContext() const { return *context_; }

bool Image::IsValid() const { return core_ && context_ && core_->IsValid(); }

void Image::Dispose() {
  if (core_) {
    core_->Dispose();
    delete core_;
    core_ = nullptr;
  }

  if (context_) {
    context_->Dispose();
    delete context_;
    context_ = nullptr;
  }
}
