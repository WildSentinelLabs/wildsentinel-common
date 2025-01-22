#pragma once

template <typename T>
struct GrayPixel {
  T g;  // Mono
};

template <typename T>
struct YAPixel {
  T g;  // Mono
  T a;  // Transparency
};
