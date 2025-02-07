#pragma once

template <typename T>
struct RGB {
  T r;  // Red
  T g;  // Green
  T b;  // Blue
};

template <typename T>
struct RGBA {
  T r;      // Red
  T g;      // Green
  T b;      // Blue
  T alpha;  // Transparency
};
