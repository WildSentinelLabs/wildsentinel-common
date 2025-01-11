#pragma once
#include "imaging/image_context.h"
#include "imaging/image_core.h"
#include "imaging/image_defs.h"

class Image {
 public:
  explicit Image(ImageCore* core);

  explicit Image(ImageCore* core, ImageContext* context);

  ~Image();

  const ImageCore& GetCore() const;

  const ImageContext& GetContext() const;

  bool IsValid() const;

  void Dispose();

 private:
  ImageCore* core_;
  ImageContext* context_;
};
