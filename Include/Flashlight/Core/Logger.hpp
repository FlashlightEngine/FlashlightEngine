// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef FL_CORE_LOGGER_HPP
#define FL_CORE_LOGGER_HPP

#include "Flashlight/Prerequisites.hpp"
#include "Flashlight/Export.hpp"

#include <string>

namespace Flashlight {
    enum class LogLevel : UInt8 {
        // Will always be reported
        Fatal = 0,
        Error = 1,

        // These one + Debug & Trace can be configured
        Warn = 2,
        Info = 3,

        // Only on debug builds, will not show in release
        Debug = 4,
        Trace = 5
    };

    class FL_API Logger {
    public:
        static bool Initialize();
        static void Shutdown();

        static void Log(LogLevel level, const char* message, ...);

    private:
    };

#ifndef FL_LOG_FATAL
    #define FL_LOG_FATAL(message, ...) Flashlight::Logger::Log(Flashlight::LogLevel::Fatal, message, ##__VA_ARGS__);
#endif

#ifndef FL_LOG_ERROR
    #define FL_LOG_ERROR(message, ...) Flashlight::Logger::Log(Flashlight::LogLevel::Error, message, ##__VA_ARGS__);
#endif

#if FL_LOG_WARN_ENABLED
    #ifndef FL_LOG_WARN
        #define FL_LOG_WARN(message, ...) Flashlight::Logger::Log(Flashlight::LogLevel::Warn, message, ##__VA_ARGS__);
    #endif
#else
    #ifndef FL_LOG_WARN
        #define FL_LOG_WARN(message, ...)
    #endif
#endif

#if FL_LOG_INFO_ENABLED
    #ifndef FL_LOG_INFO
        #define FL_LOG_INFO(message, ...) Flashlight::Logger::Log(Flashlight::LogLevel::Info, message, ##__VA_ARGS__);
    #endif
#else
    #ifndef FL_LOG_INFO
        #define FL_LOG_INFO(message, ...)
    #endif
#endif

#ifdef FL_DEBUG
    #ifdef FL_LOG_DEBUG_ENABLED
        #ifndef FL_LOG_DEBUG
            #define FL_LOG_DEBUG(message, ...) Flashlight::Logger::Log(Flashlight::LogLevel::Debug, message, ##__VA_ARGS__);
        #endif
    #else
        #ifndef FL_LOG_DEBUG
            #define FL_LOG_DEBUG(message, ...)
        #endif
    #endif
#else
    #ifndef FL_LOG_DEBUG
        #define FL_LOG_DEBUG(message, ...)
    #endif
#endif

#ifdef FL_DEBUG
    #ifdef FL_LOG_TRACE_ENABLED
        #ifndef FL_LOG_TRACE
            #define FL_LOG_TRACE(message, ...) Flashlight::Logger::Log(Flashlight::LogLevel::Trace, message, ##__VA_ARGS__);
        #endif
    #else
        #ifndef FL_LOG_TRACE
            #define FL_LOG_TRACE(message, ...)
        #endif
    #endif
#else
    #ifndef FL_LOG_TRACE
        #define FL_LOG_TRACE(message, ...)
    #endif
#endif
}

#endif // FL_CORE_LOGGER_HPP
