// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.h

#pragma once

#ifndef FL_PLATFORM_PLATFORM_H
#define FL_PLATFORM_PLATFORM_H

#include "Prerequisites.h"
#include "Export.h"

typedef struct FlPlatformState {
    void* InternalState;
} FlPlatformState;

// TODO: Stop exporting these functions.
FL_API FlBool8 flPlatformStartup(FlPlatformState* platformState,
                                 const char* applicationName,
                                 FlInt32 x,
                                 FlInt32 y,
                                 FlInt32 width,
                                 FlInt32 height);

FL_API void flPlatformShutdown(FlPlatformState* platformState);

FL_API FlBool8 flPlatformPumpMessages(FlPlatformState* platformState);

void* flPlatformAllocate(FlUInt64 size, FlBool8 aligned);
void flPlatformFree(void* block, FlBool8 aligned);
void* flPlatformZeroMemory(void* dest, FlUInt64 size);
void* flPlatformCopyMemory(void* dest, const void* source, FlUInt64 size);
void* flPlatformSetMemory(void* dest, FlInt32 value, FlUInt64 size);

void flPlatformConsoleWrite(const char* message, FlUInt8 color);
void flPlatformConsoleWriteError(const char* message, FlUInt8 color);

FlFloat64 flPlatformGetAbsoluteTime(void);

// Sleep on the thread for the provided ms. This blocks the main thread.
// Should only be used for giving time back to the OS for unused update power.
// Therefore, it is not exported.
void flPlatformSleep(FlUInt64 milliseconds);

#endif // FL_PLATFORM_PLATFORM_H
