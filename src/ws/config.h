#pragma once

#if defined(_MSC_VER)
#if defined(WSCOMMON_BUILD_DLL)
#define WSCOMMON_DLL __declspec(dllexport)
#elif defined(WSCOMMON_CONSUME_DLL)
#define WSCOMMON_DLL __declspec(dllimport)
#else
#define WSCOMMON_DLL
#endif
#else
#define WSCOMMON_DLL
#endif

#if defined(_MSVC_LANG)
#define _LANG _MSVC_LANG
#else
#define _LANG __cplusplus
#endif

#if defined(__cpp_impl_three_way_comparison) && \
    defined(__cpp_lib_three_way_comparison)
#define _CPP20_COMPARISONS_PRESENT                 \
  ((__cpp_impl_three_way_comparison >= 201907L) && \
   (__cpp_lib_three_way_comparison >= 201907L))
#else
#define _CPP20_COMPARISONS_PRESENT 0
#endif

#if __INTEL_COMPILER && (!_MSC_VER || __INTEL_CXX11_MOVE__)
#define _CPP17_DEDUCTION_GUIDES_PRESENT \
  (__INTEL_COMPILER > 2021 && _LANG >= 201703L)
#elif __clang__
#ifdef __cpp_deduction_guides
#define _CPP17_DEDUCTION_GUIDES_PRESENT (__cpp_deduction_guides >= 201611L)
#else
#define _CPP17_DEDUCTION_GUIDES_PRESENT 0
#endif
#elif __GNUC__
#define _CPP17_DEDUCTION_GUIDES_PRESENT (__cpp_deduction_guides >= 201606L)
#elif _MSC_VER
#define _CPP17_DEDUCTION_GUIDES_PRESENT    \
  (_MSC_VER >= 1914 && _LANG >= 201703L && \
   (!__INTEL_COMPILER || __INTEL_COMPILER > 2021))
#else
#define _CPP17_DEDUCTION_GUIDES_PRESENT (_LANG >= 201703L)
#endif

#if _WIN32 || _WIN64
#if defined(_M_X64) || defined(__x86_64__)
#define _x86_64 1
#elif defined(_M_IA64)
#define _ipf 1
#elif defined(_M_IX86) || defined(__i386__)
#define _x86_32 1
#else
#define _generic_arch 1
#endif
#else /* Assume generic Unix */
#if __x86_64__
#define _x86_64 1
#elif __ia64__
#define _ipf 1
#elif __i386__ || __i386
#define _x86_32 1
#else
#define _generic_arch 1
#endif
#endif
