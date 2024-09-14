// Copyright (C) Jean "Pixfri" Letessier
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.hpp

#ifndef FL_PREREQUISITES_HPP
#define FL_PREREQUISITES_HPP

// Try to identify the compiler
#if defined(__clang__)
	#define FL_COMPILER_CLANG
	#define FL_COMPILER_CLANG_VER (__clang_major__ * 100 + __clang_minor__)
	#define FL_DEPRECATED(txt) __attribute__((__deprecated__(txt)))
	#define FL_PRETTY_FUNCTION __PRETTY_FUNCTION__

	#define FL_CHECK_CLANG_VER(ver) (FL_COMPILER_CLANG_VER >= (ver))
	
	#define FL_PRAGMA(x) _Pragma(#x)

	#define FL_WARNING_CLANG_DISABLE(warn) FL_PRAGMA(clang diagnostic ignored warn)
	#define FL_WARNING_CLANG_GCC_DISABLE(warn) FL_PRAGMA(clang diagnostic ignored warn)
	#define FL_WARNING_POP() FL_PRAGMA(clang diagnostic pop)
	#define FL_WARNING_PUSH() FL_PRAGMA(clang diagnostic push)

#ifdef __MINGW32__
		#define FL_COMPILER_MINGW
#ifdef __MINGW64_VERSION_MAJOR
			#define FL_COMPILER_MINGW_W64
#endif
#endif
#elif defined(__GNUC__) || defined(__MINGW32__)
	#define FL_COMPILER_GCC
	#define FL_COMPILER_GCC_VER (__GNUC__ * 100 + __GNUC_MINOR__)
	#define FL_DEPRECATED(txt) __attribute__((__deprecated__(txt)))
	#define FL_PRETTY_FUNCTION __PRETTY_FUNCTION__

	#define FL_CHECK_GCC_VER(ver) (FL_COMPILER_GCC_VER >= ver)

	#define FL_PRAGMA(x) _Pragma(#x)

	#define FL_WARNING_CLANG_GCC_DISABLE(warn) FL_PRAGMA(GCC diagnostic ignored warn)
	#define FL_WARNING_GCC_DISABLE(warn) FL_PRAGMA(GCC diagnostic ignored warn)
	#define FL_WARNING_POP() FL_PRAGMA(GCC diagnostic pop)
	#define FL_WARNING_PUSH() FL_PRAGMA(GCC diagnostic push)

#ifdef __MINGW32__
		#define FL_COMPILER_MINGW
#ifdef __MINGW64_VERSION_MAJOR
			#define FL_COMPILER_MINGW_W64
#endif
#endif
#elif defined(__INTEL_COMPILER) || defined(__ICL)
	#define FL_COMPILER_ICC
	#define FL_COMPILER_ICC_VER __INTEL_COMPILER
	#define FL_DEPRECATED(txt) [[deprecated(txt)]]
	#define FL_PRETTY_FUNCTION __FUNCTION__
	
	#define FL_CHECK_ICC_VER(ver) (FL_COMPILER_ICC_VER >= ver)

	#define FL_PRAGMA(x) _Pragma(x)

	#define FL_WARNING_ICC_DISABLE(...) FL_PRAGMA(warning(disable: __VA_ARGS__))
	#define FL_WARNING_POP() FL_PRAGMA(warning(pop))
	#define FL_WARNING_PUSH() FL_PRAGMA(warning(push))
#elif defined(_MSC_VER)
#define FL_COMPILER_MSVC
#define FL_COMPILER_MSVC_VER _MSC_VER
#define FL_DEPRECATED(txt) __declspec(deprecated(txt))
#define FL_PRETTY_FUNCTION __FUNCSIG__

#define FL_CHECK_MSVC_VER(ver) (FL_COMPILER_MSVC_VER >= ver)

#define FL_PRAGMA(x) __pragma(x)

#define FL_WARNING_MSVC_DISABLE(...) FL_PRAGMA(warning(disable: __VA_ARGS__))
#define FL_WARNING_POP() FL_PRAGMA(warning(pop))
#define FL_WARNING_PUSH() FL_PRAGMA(warning(push))

// __cplusplus isn't respected on MSVC without /Zc:__cplusplus flag
#define FL_CPP_VER _MSVC_LANG
#else
	#define FL_COMPILER_UNKNOWN
	#define FL_DEPRECATED(txt)
	#define FL_PRETTY_FUNCTION __func__ // __func__ has been standardized in C++ 2011

	#pragma message This compiler is not fully supported
#endif

// Detect MinGW thread model
#ifdef FL_COMPILER_MINGW
#if defined(__USING_MCFGTHREAD__)
		#define FL_COMPILER_MINGW_THREADS_MCF
#elif defined(_REENTRANT)
		#define FL_COMPILER_MINGW_THREADS_POSIX
#else
		#define FL_COMPILER_MINGW_THREADS_WIN32
#endif
#endif


#ifndef FL_CHECK_CLANG_VER
#define FL_CHECK_CLANG_VER(ver) 0
#endif

#ifndef FL_CHECK_ICC_VER
#define FL_CHECK_ICC_VER(ver) 0
#endif

#ifndef FL_CHECK_GCC_VER
#define FL_CHECK_GCC_VER(ver) 0
#endif

#ifndef FL_CHECK_MSVC_VER
#define FL_CHECK_MSVC_VER(ver) 0
#endif

#ifndef FL_CPP_VER
#define FL_CPP_VER __cplusplus
#endif

#ifndef FL_PRAGMA
#define FL_PRAGMA(x) _Pragma(#x)
#endif

#ifndef FL_WARNING_CLANG_DISABLE
#define FL_WARNING_CLANG_DISABLE(warn)
#endif

#ifndef FL_WARNING_CLANG_GCC_DISABLE
#define FL_WARNING_CLANG_GCC_DISABLE(warn)
#endif

#ifndef FL_WARNING_GCC_DISABLE
#define FL_WARNING_GCC_DISABLE(warn)
#endif

#ifndef FL_WARNING_ICC_DISABLE
#define FL_WARNING_ICC_DISABLE(warn)
#endif

#ifndef FL_WARNING_MSVC_DISABLE
#define FL_WARNING_MSVC_DISABLE(...)
#endif

#ifndef FL_WARNING_POP
#define FL_WARNING_POP()
#endif

#ifndef FL_WARNING_PUSH
#define FL_WARNING_PUSH()
#endif


// C++ version
#define FL_CPP98 199711L
#define FL_CPP11 201103L
#define FL_CPP14 201402L
#define FL_CPP17 201703L
#define FL_CPP20 202002L
#define FL_CPP23 202302L

#define FL_CHECK_CPP_VER(ver) (FL_CPP_VER >= (ver))

#if !FL_CHECK_CPP_VER(FL_CPP17)
	#error Flashlight Engine requires C++17 or higher
#endif

// Try to identify target platform via defines
#if defined(_WIN32)
#define FL_PLATFORM_WINDOWS

#ifndef _WIN64
#error "64-bit is required to build Flashlight Engine."
#endif

#define FL_EXPORT __declspec(dllexport)
#define FL_IMPORT __declspec(dllimport)

// Some defines for windows.h include...
#if defined(FL_BUILD)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

#if FLUTILS_WINDOWS_NT6
#define FL_WINNT 0x0600
#else
			#define FL_WINNT 0x0501
#endif

// Keep the actual define if existing and greater than our requirement
#if defined(_WIN32_WINNT)
#if _WIN32_WINNT < FL_WINNT
				#undef _WIN32_WINNT
				#define _WIN32_WINNT FL_WINNT
#endif
#else
#define _WIN32_WINNT FL_WINNT
#endif
#endif
#elif defined(__linux__)
	#define FL_PLATFORM_LINUX

	#define FL_EXPORT __attribute__((visibility("default")))
	#define FL_IMPORT __attribute__((visibility("default")))

#if defined(__ANDROID__)
		#define FL_PLATFORM_ANDROID
#endif
#elif defined (__unix__)
// Catch anything not caught by the above
#define FL_PLATFORM_UNIX
#elif __APPLE__
// Apple platforms
#define FL_PLATFORM_APPLE
#include <TargetConditionals.h>
#if TARGET_IPHONE_SIMULATOR
// iOS simulator
#define FL_PLATFORM_IOS
#define FL_PLATFORM_IOS_SIMULATOR
#elif TARGET_OS_IPHONE
// iOS device
#define FL_PLATFORM_IOS
#elif TARGET_OS_MAC
// macOS
#else
#error "Unknown Apple platform"
#endif
#else
	#pragma message "This operating system is not fully supported by Flashlight Engine"

	#define FL_PLATFORM_UNKNOWN
	#define FL_EXPORT
	#define FL_IMPORT
#endif

#ifndef FL_CHECK_NDK_VER
#define FL_CHECK_NDK_VER(ver) 0
#endif

// Feature checking
#ifdef __has_cpp_attribute
#define FL_HAS_CPP_ATTRIBUTE(attr) __has_cpp_attribute(attr)
#else
	#define FL_HAS_CPP_ATTRIBUTE(attr) (0)
#endif

// "Assume" attribute
#ifndef FL_NO_ASSUME_ATTRIBUTE

#if FL_CHECK_CPP_VER(FL_CPP23) || FL_HAS_CPP_ATTRIBUTE(assume)
	#define FL_ASSUME(expr) [[assume(expr)]]
#endif

#ifndef FL_ASSUME

#if defined(FL_COMPILER_CLANG)
		#define FL_ASSUME(expr) __builtin_assume(expr)
#endif

#if defined(FL_COMPILER_GCC)

// __attribute__(assume) is supported starting with GCC 13
#if __GNUC__ >= 13
			#define FL_ASSUME(expr) __attribute__(assume(expr))
#endif

#endif

#if defined(FL_COMPILER_MSVC)
#define FL_ASSUME(expr) __assume(expr)
#endif

#endif

#endif // FL_NO_ASSUME_ATTRIBUTE

// "Force inline" attribute
#ifndef FL_NO_FORCEINLINE_ATTRIBUTE

#ifndef FL_FORCEINLINE

#if defined(FL_COMPILER_CLANG) || defined(FL_COMPILER_GCC)
		#define FL_FORCEINLINE __attribute__((always_inline)) inline
#endif

#if defined(FL_COMPILER_MSVC)
#define FL_FORCEINLINE __forceinline
#endif

#endif

#endif // FL_NO_FORCEINLINE_ATTRIBUTE

// "Likely"/"unlikely" attributes
#ifndef FL_NO_LIKELY_ATTRIBUTE

#if FL_CHECK_CPP_VER(FL_CPP20) || FL_HAS_CPP_ATTRIBUTE(likely)
#define FL_LIKELY(expr) (expr) [[likely]]
#endif

#if FL_CHECK_CPP_VER(FL_CPP20) || FL_HAS_CPP_ATTRIBUTE(unlikely)
#define FL_UNLIKELY(expr) (expr) [[unlikely]]
#endif

#if defined(FL_COMPILER_CLANG) || defined(FL_COMPILER_GCC) || defined(FL_COMPILER_INTEL)

#ifndef FL_LIKELY
		#define FL_LIKELY(expr) (__builtin_expect(!!(expr), 1))
#endif

#ifndef FL_UNLIKELY
		#define FL_UNLIKELY(expr) (__builtin_expect(!!(expr), 0))
#endif

#endif

#endif // FL_NO_LIKELY_ATTRIBUTE

// Unreachable macro
#ifndef FL_NO_UNREACHABLE_MACRO

#if defined(FL_COMPILER_CLANG) || defined(FL_COMPILER_GCC) || defined(FL_COMPILER_INTEL)
	#define FL_UNREACHABLE() __builtin_unreachable()
#elif defined(FL_COMPILER_MSVC)
#define FL_UNREACHABLE() __assume(false)
#endif

#endif // FL_NO_UNREACHABLE_MACRO

// Fallbacks

#ifndef FL_ASSUME
	#define FL_ASSUME(expr)
#endif

#ifndef FL_FORCEINLINE
	#define FL_FORCEINLINE inline
#endif

#ifndef FL_LIKELY
	#define FL_LIKELY(expr) (expr)
#endif

#ifndef FL_UNLIKELY
	#define FL_UNLIKELY(expr) (expr)
#endif

#ifndef FL_UNREACHABLE
	#define FL_UNREACHABLE() throw Flashlight::UnreachableError{};
#endif

// Detect arch
#ifndef FL_NO_ARCH_DETECTION

#if defined(__arm__) || defined(__thumb__) || defined(__ARM_ARCH_7__) || defined(_M_ARM)
		#define FL_ARCH_arm
#endif

#if defined(__aarch64__) || defined(_M_ARM64) || defined(_M_ARM64EC) || defined(__arm64__)
		#define FL_ARCH_aarch64
#endif

#if defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64) || defined(_M_AMD64) || defined (_M_X64)
#define FL_ARCH_x86_64
#endif

#if defined(__i386__) || defined(_M_IX86) || defined(_X86_)
		#define FL_ARCH_x86
#endif

#if defined(__wasm32__)
		#define FL_ARCH_wasm32
#endif

#if defined(__wasm64__)
		#define FL_ARCH_wasm64
#endif

#if defined(FL_ARCH_arm) \
	  + defined(FL_ARCH_aarch64) \
	  + defined(FL_ARCH_x86_64) \
	  + defined(FL_ARCH_x86) \
	  + defined(FL_ARCH_wasm32) \
	  + defined(FL_ARCH_wasm64) \
    != 1

	#error No or multiple arch detected! Please open an issue with details about your target system. You can define FL_NO_ARCH_DETECTION to bypass this error.

#endif

#endif // FL_NO_ARCH_DETECTION

// A bunch of useful macros
#define FLPrefix(a, prefix) prefix ## a
#define FLPrefixMacro(a, prefix) FLPrefix(a, prefix)
#define FLSuffix(a, suffix) a ## suffix
#define FLSuffixMacro(a, suffix) FLSuffix(a, suffix)
#define FLStringify(s) #s
#define FLStringifyMacro(s) FLStringify(s) // http://gcc.gnu.org/onlinedocs/cpp/Stringification.html#Stringification
#define FLUnused(a) (void) a

#include <climits>
#include <cstdint>

static_assert(CHAR_BIT == 8, "CHAR_BIT is expected to be 8");

static_assert(sizeof(int8_t) == 1, "int8_t is not of the correct size");
static_assert(sizeof(int16_t) == 2, "int16_t is not of the correct size");
static_assert(sizeof(int32_t) == 4, "int32_t is not of the correct size");
static_assert(sizeof(int64_t) == 8, "int64_t is not of the correct size");

static_assert(sizeof(uint8_t) == 1, "uint8_t is not of the correct size");
static_assert(sizeof(uint16_t) == 2, "uint16_t is not of the correct size");
static_assert(sizeof(uint32_t) == 4, "uint32_t is not of the correct size");
static_assert(sizeof(uint64_t) == 8, "uint64_t is not of the correct size");

namespace Flashlight {
    using Int8 = int8_t;
    using UInt8 = uint8_t;

    using Int16 = int16_t;
    using UInt16 = uint16_t;

    using Int32 = int32_t;
    using UInt32 = uint32_t;

    using Int64 = int64_t;
    using UInt64 = uint64_t;

    using USize = size_t;

    using Float32 = float;
    using Float64 = double;

    struct UnreachableError {
    };
}

#endif // FL_PREREQUISITES_HPP
