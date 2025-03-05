#pragma once

#if defined(__cpp_impl_three_way_comparison) && \
    defined(__cpp_lib_three_way_comparison)
#define __CPP20_COMPARISONS_PRESENT                \
  ((__cpp_impl_three_way_comparison >= 201907L) && \
   (__cpp_lib_three_way_comparison >= 201907L))
#else
#define __CPP20_COMPARISONS_PRESENT 0
#endif

#if __INTEL_COMPILER && (!_MSC_VER || __INTEL_CXX11_MOVE__)
#define __CPP17_DEDUCTION_GUIDES_PRESENT \
  (__INTEL_COMPILER > 2021 && __TBB_LANG >= 201703L)
#elif __clang__
#ifdef __cpp_deduction_guides
#define __CPP17_DEDUCTION_GUIDES_PRESENT (__cpp_deduction_guides >= 201611L)
#else
#define __CPP17_DEDUCTION_GUIDES_PRESENT 0
#endif
#elif __GNUC__
#define __CPP17_DEDUCTION_GUIDES_PRESENT (__cpp_deduction_guides >= 201606L)
#elif _MSC_VER
#define __CPP17_DEDUCTION_GUIDES_PRESENT        \
  (_MSC_VER >= 1914 && __TBB_LANG >= 201703L && \
   (!__INTEL_COMPILER || __INTEL_COMPILER > 2021))
#else
#define __CPP17_DEDUCTION_GUIDES_PRESENT (__TBB_LANG >= 201703L)
#endif
