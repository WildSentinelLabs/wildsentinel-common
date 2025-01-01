#pragma once
#include "imaging/image_context.h"
#include "imaging/image_defs.h"
#include "imaging/pixel_buffer.h"

class Image {
 public:
  Image(IPixelBuffer* buffer, const ImageFormat& format);

  Image(IPixelBuffer* buffer, ImageContext* context);

  ~Image();

  const IPixelBuffer& GetPixelBuffer() const;

  const ImageContext& GetContext() const;

  bool IsValid() const;

  void Dispose();

 private:
  IPixelBuffer* pixel_buffer_;
  ImageContext* context_;
};
