// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.h

#include "Flashlight/Core/Logger.h"

#include "Flashlight/Core/Asserts.h"

// TODO: Temporary
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

void ReportAssertionFailure(const char* expression, const char* message, const char* file, Int32 line) {
    LogOutput(LogLevelFatal, "Assertion failed: %s, message: '%s', in file %s, line: %d\n",
              expression, message, file, line);
}

Bool8 InitializeLogging(void) {
    // TODO: Create log file.
    return TRUE;
}

void ShutdownLogging(void) {
    // TODO: Clean up logging/writing queued entries.
}

void LogOutput(const LogLevel level, const char* message, ...) {
    const char* levelStrings[6] = {
        "[FATAL]: ",
        "[ERROR]: ",
        "[WARN]:  ",
        "[INFO]:  ",
        "[DEBUG]: ",
        "[TRACE]: "
    };
    // Bool8 isError = level <= 2;

    // Technically imposes a 32k character limit on a single log entry, but...
    // DON'T DO THAT
    char out_message[32000];
    memset(out_message, 0, sizeof(out_message));

    // Format original message.
    va_list arg_ptr;
    va_start(arg_ptr, message);
    vsnprintf(out_message, 32000, message, arg_ptr);
    va_end(arg_ptr);

    char out_message2[32000];
    sprintf(out_message2, "%s%s\n", levelStrings[level], out_message);

    // TODO: platform-specific output.
    printf("%s", out_message2);
}
