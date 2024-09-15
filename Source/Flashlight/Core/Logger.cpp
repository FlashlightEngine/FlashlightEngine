// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.hpp

#include "Flashlight/Core/Logger.hpp"

#include <cstdarg>
#include <cstring>
#include <unordered_map>

namespace Flashlight {
    bool Logger::Initialize() {
        // TODO: Create log file.
        return true;
    }

    void Logger::Shutdown() {
        // TODO: Clean up logging/writing queued entries.
    }

    void Logger::Log(const LogLevel level, const char* message, ...) {
        const char* levelStrings[6] = {
            "[FATAL]: ",
            "[ERROR]: ",
            "[WARN]:  ",
            "[INFO]:  ",
            "[DEBUG]: ",
            "[TRACE]: "
        };
        // const bool isError = level <= LogLevel::Error;

        // Technically imposes a 32k character limit on a single log entry, but...
        // DON'T DO THAT
        char out_message[32000];
        memset(out_message, 0, sizeof(out_message));

        // Format original message.
        // NOTE: Oddly enough, MS's headers override the GCC/Clang va_list type with a "typedef char* va_list" in some
        // cases, and as a result throws a strange error here. The workaround for now is to just use __builtin_va_list,
        // which is the type GCC/Clang's va_start expects.
        va_list arg_ptr;
        va_start(arg_ptr, message);
        vsnprintf(out_message, 32000, message, arg_ptr);
        va_end(arg_ptr);

        char out_message2[32000];
        sprintf(out_message2, "%s%s\n", levelStrings[static_cast<UInt8>(level)], out_message);

        // TODO: platform-specific output.
        printf("%s", out_message2);
    }
}
