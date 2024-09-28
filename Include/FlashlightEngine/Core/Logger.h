// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef FL_CORE_LOGGER_H
#define FL_CORE_LOGGER_H

#include "FlashlightEngine/Prerequisites.h"
#include "FlashlightEngine/Export.h"

typedef enum FlLogLevel {
    FlLogLevelFatal = 0,
    FlLogLevelError = 1,
    FlLogLevelWarn = 2,
    FlLogLevelInfo = 3,
    FlLogLevelDebug = 4,
    FlLogLevelTrace = 5
} FlLogLevel;

FlBool8 flInitializeLogging(void);
void flShutdownLogging(void);

FL_API void flLogOutput(FlLogLevel level, const char* message, ...);

#ifndef FL_LOG_FATAL
    #define FL_LOG_FATAL(message, ...) flLogOutput(FlLogLevelFatal, message, ##__VA_ARGS__);
#endif

#ifndef FL_LOG_ERROR
    #define FL_LOG_ERROR(message, ...) flLogOutput(FlLogLevelError, message, ##__VA_ARGS__);
#endif

#ifdef FL_LOG_WARN_ENABLED
    #ifndef FL_LOG_WARN
        #define FL_LOG_WARN(message, ...) flLogOutput(FlLogLevelWarn, message, ##__VA_ARGS__);
    #endif
#else
    #ifndef FL_LOG_WARN
        #define FL_LOG_WARN(message, ...)
    #endif
#endif

#ifdef FL_LOG_INFO_ENABLED
    #ifndef FL_LOG_INFO
        #define FL_LOG_INFO(message, ...) flLogOutput(FlLogLevelInfo, message, ##__VA_ARGS__);
    #endif
#else
    #ifndef FL_LOG_INFO
        #define FL_LOG_INFO(message, ...)
    #endif
#endif

#ifdef FL_DEBUG
    #ifdef FL_LOG_DEBUG_ENABLED
        #ifndef FL_LOG_DEBUG
            #define FL_LOG_DEBUG(message, ...) flLogOutput(FlLogLevelDebug, message, ##__VA_ARGS__);
        #endif
    #else
        #ifndef FL_LOG_DEBUG
            #define FL_LOG_DEBUG(message, ...)
        #endif
    #endif

    #ifdef FL_LOG_TRACE_ENABLED
        #ifndef FL_LOG_TRACE
            #define FL_LOG_TRACE(message, ...) flLogOutput(FlLogLevelTrace, message, ##__VA_ARGS__);
        #endif
    #else
        #ifndef FL_LOG_TRACE
            #define FL_LOG_TRACE(message, ...)
        #endif
    #endif
#else
    #ifndef FL_LOG_DEBUG
        #define FL_LOG_DEBUG(message, ...)
    #endif
    
    #ifndef FL_LOG_TRACE
        #define FL_LOG_TRACE(message, ...)
    #endif
#endif

#endif // FL_CORE_LOGGER_H
