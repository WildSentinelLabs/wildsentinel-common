#pragma once

template <typename T>
struct RGBPixel {
  T r;  // Red
  T g;  // Green
  T b;  // Blue
};

template <typename T>
struct RGBAPixel {
  T r;  // Red
  T g;  // Green
  T b;  // Blue
  T a;  // Transparency
};
