#pragma once
#include "imaging/image_context.h"
#include "imaging/image_core.h"
#include "imaging/image_defs.h"

class Image {
 public:
  explicit Image(ImageCore* buffer);

  explicit Image(ImageCore* buffer, ImageContext* context);

  ~Image();

  const ImageCore& GetPixelBuffer() const;

  const ImageContext& GetContext() const;

  bool IsValid() const;

  void Dispose();

 private:
  ImageCore* pixel_buffer_;
  ImageContext* context_;
};
