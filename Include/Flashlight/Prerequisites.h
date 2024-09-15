// Copyright (C) Jean "Pixfri" Letessier
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.h

#pragma once

#ifndef FL_PREREQUISITES_H
#define FL_PREREQUISITES_H

// Unsigned integer types
typedef unsigned char UInt8;
typedef unsigned short UInt16;
typedef unsigned int UInt32;
typedef unsigned long long UInt64;

// Signed integer types
typedef signed char Int8;
typedef signed short Int16;
typedef signed int Int32;
typedef signed long long Int64;

// Floating point types
typedef float Float32;
typedef double Float64;

// Boolean
typedef int Bool32;
typedef char Bool8;

// Properly define static assertions.
#if defined(__clang__) || defined(__gcc__)
#define STATIC_ASSERT _Static_assert
#else
#define STATIC_ASSERT static_assert
#endif

// Ensure all types are of the correct size
STATIC_ASSERT(sizeof(UInt8) == 1, "Expected UInt8 to be 1 byte.");
STATIC_ASSERT(sizeof(UInt16) == 2, "Expected UInt16 to be 2 bytes.");
STATIC_ASSERT(sizeof(UInt32) == 4, "Expected UInt32 to be 4 bytes.");
STATIC_ASSERT(sizeof(UInt64) == 8, "Expected UInt64 to be 8 bytes.");

STATIC_ASSERT(sizeof(Int8) == 1, "Expected Int8 to be 1 byte.");
STATIC_ASSERT(sizeof(Int16) == 2, "Expected Int16 to be 2 bytes.");
STATIC_ASSERT(sizeof(Int32) == 4, "Expected Int32 to be 4 bytes.");
STATIC_ASSERT(sizeof(Int64) == 8, "Expected Int64 to be 8 bytes.");

STATIC_ASSERT(sizeof(Float32) == 4, "Expected Float32 to be 4 bytes.");
STATIC_ASSERT(sizeof(Float64) == 8, "Expected Float64 to be 8 bytes.");

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

#endif // FL_PREREQUISITES_H