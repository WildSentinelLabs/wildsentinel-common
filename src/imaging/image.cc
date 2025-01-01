#include "imaging/image.h"

Image::Image(IPixelBuffer* buffer, const ImageFormat& format)
    : pixel_buffer_(buffer), context_(new ImageContext(format)) {}

Image::Image(IPixelBuffer* buffer, ImageContext* context)
    : pixel_buffer_(buffer), context_(context) {}

Image::~Image() { Dispose(); }

const IPixelBuffer& Image::GetPixelBuffer() const { return *pixel_buffer_; }

const ImageContext& Image::GetContext() const { return *context_; }

bool Image::IsValid() const {
  return pixel_buffer_ && context_ && pixel_buffer_->IsValid();
}

void Image::Dispose() {
  if (pixel_buffer_) {
    pixel_buffer_->Dispose();
    delete pixel_buffer_;
    pixel_buffer_ = nullptr;
  }

  if (context_) {
    context_->Dispose();
    delete context_;
    context_ = nullptr;
  }
}