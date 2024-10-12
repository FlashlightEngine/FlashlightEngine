// Copyright (C) Jean "Pixfri" Letessier
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.h

#pragma once

#ifndef FL_PREREQUISITES_H
#define FL_PREREQUISITES_H

// Unsigned integer types
typedef unsigned char FlUInt8;
typedef unsigned short FlUInt16;
typedef unsigned int FlUInt32;
typedef unsigned long long FlUInt64;

// Signed integer types
typedef signed char FlInt8;
typedef signed short FlInt16;
typedef signed int FlInt32;
typedef signed long long FlInt64;

// Floating point types
typedef float FlFloat32;
typedef double FlFloat64;

// Boolean
typedef int FlBool32;
typedef _Bool FlBool8;

// Properly define static assertions.
#if defined(__clang__) || defined(__gcc__)
#define STATIC_ASSERT _Static_assert
#else
#define STATIC_ASSERT static_assert
#endif

// Ensure all types are of the correct size
STATIC_ASSERT(sizeof(FlUInt8) == 1, "Expected FlUInt8 to be 1 byte.");
STATIC_ASSERT(sizeof(FlUInt16) == 2, "Expected FlUInt16 to be 2 bytes.");
STATIC_ASSERT(sizeof(FlUInt32) == 4, "Expected FlUInt32 to be 4 bytes.");
STATIC_ASSERT(sizeof(FlUInt64) == 8, "Expected FlUInt64 to be 8 bytes.");

STATIC_ASSERT(sizeof(FlInt8) == 1, "Expected FlInt8 to be 1 byte.");
STATIC_ASSERT(sizeof(FlInt16) == 2, "Expected FlInt16 to be 2 bytes.");
STATIC_ASSERT(sizeof(FlInt32) == 4, "Expected FlInt32 to be 4 bytes.");
STATIC_ASSERT(sizeof(FlInt64) == 8, "Expected FlInt64 to be 8 bytes.");

STATIC_ASSERT(sizeof(FlFloat32) == 4, "Expected FlFloat32 to be 4 bytes.");
STATIC_ASSERT(sizeof(FlFloat64) == 8, "Expected FlFloat64 to be 8 bytes.");

#define true 1
#define false 0

// Platform detection
#if defined(FL_PLATFORM_WINDOWS)
    #ifndef _WIN64
        #error "Flashlight Engine only supports 64-bit windows."
    #endif
#elif defined(FL_PLATFORM_APPLE)
    #include <TargetConditionals.h>

    #if TARGET_SIMULATOR_IPHONE
        #define FL_PLATFORM_IOS_SIMULATOR
    #endif
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

#ifdef FL_PLATFORM_LINUX
    #ifndef _POSIX_C_SOURCE
        #define _POSIX_C_SOURCE 199309L
    #endif
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

#endif // FL_PREREQUISITES_H