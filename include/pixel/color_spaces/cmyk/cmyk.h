#pragma once

template <typename T>
struct CMYK {
  T c;  // Cyan
  T m;  // Magenta
  T y;  // Yellow
  T k;  // Black
};

template <typename T>
struct CMYKA {
  T c;      // Cyan
  T m;      // Magenta
  T y;      // Yellow
  T k;      // Black
  T alpha;  // Transparency
};
