// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef FL_CORE_LOGGER_H
#define FL_CORE_LOGGER_H

#include "FlashlightEngine/Prerequisites.h"
#include "FlashlightEngine/Export.h"

typedef enum FlLogLevel {
    LogLevelFatal = 0,
    LogLevelError = 1,
    LogLevelWarn = 2,
    LogLevelInfo = 3,
    LogLevelDebug = 4,
    LogLevelTrace = 5
} FlLogLevel;

FlBool8 flInitializeLogging(void);
void flShutdownLogging(void);

FL_API void flLogOutput(FlLogLevel level, const char* message, ...);

#ifndef FL_LOG_FATAL
    #define FL_LOG_FATAL(message, ...) flLogOutput(LogLevelFatal, message, ##__VA_ARGS__);
#endif

#ifndef FL_LOG_ERROR
    #define FL_LOG_ERROR(message, ...) flLogOutput(LogLevelError, message, ##__VA_ARGS__);
#endif

#ifdef FL_LOG_WARN_ENABLED
    #ifndef FL_LOG_WARN
        #define FL_LOG_WARN(message, ...) flLogOutput(LogLevelWarn, message, ##__VA_ARGS__);
    #endif
#else
    #ifndef FL_LOG_WARN
        #define FL_LOG_WARN(message, ...)
    #endif
#endif

#ifdef FL_LOG_INFO_ENABLED
    #ifndef FL_LOG_INFO
        #define FL_LOG_INFO(message, ...) flLogOutput(LogLevelInfo, message, ##__VA_ARGS__);
    #endif
#else
    #ifndef FL_LOG_INFO
        #define FL_LOG_INFO(message, ...)
    #endif
#endif

#ifdef FL_DEBUG
    #ifdef FL_LOG_DEBUG_ENABLED
        #ifndef FL_LOG_DEBUG
            #define FL_LOG_DEBUG(message, ...) flLogOutput(LogLevelDebug, message, ##__VA_ARGS__);
        #endif
    #else
        #ifndef FL_LOG_DEBUG
            #define FL_LOG_DEBUG(message, ...)
        #endif
    #endif

    #ifdef FL_LOG_TRACE_ENABLED
        #ifndef FL_LOG_TRACE
            #define FL_LOG_TRACE(message, ...) flLogOutput(LogLevelTrace, message, ##__VA_ARGS__);
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
