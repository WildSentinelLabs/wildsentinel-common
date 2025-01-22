#pragma once

template <typename T>
struct CMYKPixel {
  T c;  // Cyan
  T m;  // Magenta
  T y;  // Yellow
  T k;  // Black
};

template <typename T>
struct CMYKAPixel {
  T c;  // Cyan
  T m;  // Magenta
  T y;  // Yellow
  T k;  // Black
  T a;  // Transparency
};
