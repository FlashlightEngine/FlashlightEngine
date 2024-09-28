// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.h

#include "FlashlightEngine/Core/Logger.h"

#include "FlashlightEngine/Core/Asserts.h"
#include "FlashlightEngine/Platform/Platform.h"

// TODO: Temporary
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

void flReportAssertionFailure(const char* expression, const char* message, const char* file, FlInt32 line) {
    flLogOutput(FlLogLevelFatal, "Assertion failed: %s, message: '%s', in file %s, line: %d\n",
                expression, message, file, line);
}

FlBool8 flInitializeLogging(void) {
    // TODO: Create log file.
    return TRUE;
}

void flShutdownLogging(void) {
    // TODO: Clean up logging/writing queued entries.
}

void flLogOutput(const FlLogLevel level, const char* message, ...) {
    const char* levelStrings[6] = {
        "[FATAL]: ",
        "[ERROR]: ",
        "[WARN]:  ",
        "[INFO]:  ",
        "[DEBUG]: ",
        "[TRACE]: "
    };
    const FlBool8 isError = level < FlLogLevelWarn;

    // Technically imposes a 32k character limit on a single log entry, but...
    // DON'T DO THAT
    char outMessage[32000];
    memset(outMessage, 0, sizeof(outMessage));

    // Format original message.
    va_list arg_ptr;
    va_start(arg_ptr, message);
    vsnprintf(outMessage, 32000, message, arg_ptr);
    va_end(arg_ptr);

    char outMessage2[32000];
    sprintf(outMessage2, "%s%s\n", levelStrings[level], outMessage);

    // Platform specific output
    if (isError) {
        flPlatformConsoleWriteError(outMessage2, (FlUInt8)level);
    } else {
        flPlatformConsoleWrite(outMessage2, (FlUInt8)level);
    }
}
