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
typedef char FlBool8;

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
#define FL_IMPORT __declspec(dllexport)
#else
#define FL_EXPORT __attribute__((visibility("default")))
#define FL_IMPORT __attribute__((visibility("default")))
#endif

#define FL_UNUSED_PARAM(param) (void)(param)

#endif // FL_PREREQUISITES_H