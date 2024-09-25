// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef FL_CORE_ASSERTS_HPP
#define FL_CORE_ASSERTS_HPP

#include "FlashlightEngine/Prerequisites.h"
#include "FlashlightEngine/Export.h"

#ifdef FL_ASSERTIONS_ENABLED
    #ifdef _MSC_VER
        #include <intrin.h>
        #define DebugBreak() __debugbreak()
    #else
        #define DebugBreak() __builtin_trap()
    #endif

    FL_API void flReportAssertionFailure(const char* expression, const char* message, const char* file, FlInt32 line);

#define FL_ASSERT(expr)                                              \
    {                                                                \
        if (expr) {                                                  \
        } else {                                                     \
            flReportAssertionFailure(#expr, "", __FILE__, __LINE__); \
            DebugBreak();                                            \
        }                                                            \
    }


#define FL_ASSERT_MSG(expr, message)                                      \
    {                                                                     \
        if (expr) {                                                       \
        } else {                                                          \
            flReportAssertionFailure(#expr, message, __FILE__, __LINE__); \
            DebugBreak();                                                 \
        }                                                                 \
    }

#ifdef FL_DEBUG
#define FL_ASSERT_DEBUG(expr)                                        \
    {                                                                \
        if (expr) {                                                  \
        } else {                                                     \
            flReportAssertionFailure(#expr, "", __FILE__, __LINE__); \
            DebugBreak();                                            \
        }                                                            \
    }
#else
#define FL_ASSERT_DEBUG(expr) // Does nothing
#endif

#else
#define FL_ASSERT(expr)              // Does nothing
#define FL_ASSERT_MSG(expr, message) // Does nothing
#define FL_ASSERT_DEBUG(expr)        // Does nothing
#endif

#endif // FL_CORE_ASSERTS_HPP
