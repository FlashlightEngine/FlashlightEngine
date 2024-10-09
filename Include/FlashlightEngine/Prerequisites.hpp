// Copyright (C) Jean "Pixfri" Letessier
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef FL_PREREQUISITES_HPP
#define FL_PREREQUISITES_HPP

#include <cstdint>

namespace Fl {

// Unsigned integer types
using UInt8  = std::uint8_t;
using UInt16 = std::uint16_t;
using UInt32 = std::uint32_t;
using UInt64 = std::uint64_t;

// Signed integer types
using Int8  = std::int8_t;
using Int16 = std::int16_t;
using Int32 = std::int32_t;
using Int64 = std::int64_t;

// Floating point types
using Float32 = float;
using Float64 = double;

// Ensure all types are of the correct size
static_assert(sizeof(UInt8) == 1, "Expected Fl::UInt8 to be 1 byte.");
static_assert(sizeof(UInt16) == 2, "Expected Fl::UInt16 to be 2 bytes.");
static_assert(sizeof(UInt32) == 4, "Expected Fl::UInt32 to be 4 bytes.");
static_assert(sizeof(UInt64) == 8, "Expected Fl::UInt64 to be 8 bytes.");

static_assert(sizeof(Int8) == 1, "Expected Fl::Int8 to be 1 byte.");
static_assert(sizeof(Int16) == 2, "Expected Fl::Int16 to be 2 bytes.");
static_assert(sizeof(Int32) == 4, "Expected Fl::Int32 to be 4 bytes.");
static_assert(sizeof(Int64) == 8, "Expected Fl::Int64 to be 8 bytes.");

static_assert(sizeof(Float32) == 4, "Expected Fl::Float32 to be 4 bytes.");
static_assert(sizeof(Float64) == 8, "Expected Fl::Float64 to be 8 bytes.");

#define TRUE 1
#define FALSE 0

// Platform detection
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
    #define FL_PLATFORM_WINDOWS
    #ifndef _WIN64
        #error "Flashlight Engine only supports 64-bit windows."
    #endif
#elif defined(__linux__) || defined(__gnu_linux__)
    #define FL_PLATFORM_LINUX
    #if defined(__ANDROID__)
        #define FL_PLATFORM_ANDROID
    #endif
#elif defined(__unix__)
    // Catch anything not caught by the above
    #define FL_PLATFORM_UNIX
#elif defined(_POSIX_VERSION)
    // Posix
    #define FL_PLATFORM_POSIX
#elif __APPLE__
    // Apple platforms
    #define FL_PLATFORM_APPLE

    #include <TargetConditionals.h>
    #if TARGET_SIMULATOR_IPHONE
        // iOS simulator
        #define FL_PLATFORM_IOS_SIMULATOR
        #define FL_PLATFORM_IOS
    #elif TARGET_OS_IPHONE
        // iOS device
        #define FL_PLATFORM_IOS
    #elif TARGET_OS_MAC
        // Other kinds of macOS
    #else
        #error "Unknown Apple platform"
    #endif
#else
#error "Unknown platform"
#endif

#ifdef _MSC_VER
#define FL_EXPORT __declspec(dllexport)
#define FL_IMPORT __declspec(dllimport)
#else
#define FL_EXPORT __attribute__((visibility("default")))
#define FL_IMPORT __attribute__((visibility("default")))
#endif

#ifdef _MSC_VER
    #ifdef __cplusplus
        #define FL_TYPEOF(x) decltype(x)
    #else
        #define FL_TYPEOF(x) __typeof__(x)
    #endif
#else
    #define FL_TYPEOF(x) typeof(x)
#endif

#ifdef _MSC_VER
#define FL_INLINE __forceinline
#define FL_NO_INLINE __declspec(noinline)
#else
#define FL_INLINE static inline
#define FL_NO_INLINE
#endif

#define FL_UNUSED(a) (void)(a)
#define FL_CLAMP(value, min, max) (value <= min) ? min : (value >= max) ? max : value

}

#endif // FL_PREREQUISITES_HPP
