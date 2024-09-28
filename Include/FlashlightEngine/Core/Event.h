// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.h

#pragma once

#ifndef FL_CORE_EVENT_H
#define FL_CORE_EVENT_H

#include "FlashlightEngine/Prerequisites.h"
#include "FlashlightEngine/Export.h"

typedef struct FlEventContext {
    // 128 bits
    union {
        FlInt64 int64[2];
        FlUInt64 uint64[2];
        FlFloat64 float64[2];

        FlInt32 int32[4];
        FlUInt32 uint32[4];
        FlFloat32 float32[4];

        FlInt16 int16[8];
        FlUInt16 uint16[8];

        FlInt8 int8[16];
        FlUInt8 uint8[16];

        char c[16];
    } data;
} FlEventContext;

// Should return true if handled.
typedef FlBool8 (*PFN_flOnEvent)(FlUInt16 code, void* sender, void* listenerInstance, FlEventContext data);

FlBool8 flEventInitialize(void);
void flEventShutdown(void);

/**
 * Register to listen for when events are sent with the provided code. Events with duplicate
 * listener/callback combos will not be registered again and will cause this to return FALSE.
 * @param code The event code to listen for.
 * @param listener A pointer to a listener instance. Can be 0/NULL.
 * @param onEvent The callback function pointer to be invoked when the event code is fired.
 * @returns TRUE if the event is successfully registered, otherwise FALSE.
 */
FL_API FlBool8 flEventRegister(FlUInt16 code, void* listener, PFN_flOnEvent onEvent);

/**
 * Unregister from listening for when events are sent with the provided code. If no matching
 * registration is found, this function returns FALSE.
 * @param code The event code to stop listening for.
 * @param listener A pointer to a listener instance. Can be 0/NULL.
 * @param onEvent The callback function pointer to be unregistered.
 * @returns TRUE if the event is successfully unregistered, otherwise FALSE.
 */
FL_API FlBool8 flEventUnregister(FlUInt16 code, void* listener, PFN_flOnEvent onEvent);

/**
 * Fires an event to listeners of the given code. If an event handler returns
 * TRUE, the event is considered handled and is not passed on to any more listeners.
 * @param code The event code to fire.
 * @param sender A pointer to the sender. Can be 0/NULL.
 * @param data The event data.
 * @returns TRUE if handled, otherwise FALSE.
 */
FL_API FlBool8 flEventFire(FlUInt16 code, void* sender, FlEventContext eventContext);

// System internal event codes. Application should use codes beyond 255.
typedef enum FlSystemEventCode {
    // Shuts the application down on the next frame.
    FlEventCodeApplicationQuit = 0x01,

    // Keyboard key pressed.
    /* Context usage:
     * FlUInt16 keyCode = data.data.uint16[0];
     */
    FlEventCodeKeyPressed = 0x02,

    // Keyboard key released.
    /* Context usage:
     * FlUInt16 keyCode = data.data.uint16[0];
     */
    FlEventCodeKeyReleased = 0x03,

    // Mouse button pressed.
    /* Context usage:
     * FlUInt16 button = data.data.uint16[0];
     */
    FlEventCodeButtonPressed = 0x04,

    // Mouse button released.
    /* Context usage:
     * FlUInt16 button = data.data.uint16[0];
     */
    FlEventCodeButtonReleased = 0x05,

    // Mouse moved.
    /* Context usage:
     * FlUInt16 x = data.data.uint16[0];
     * FlUInt16 y = data.data.uint16[1];
     */
    FlEventCodeMouseMoved = 0x06,

    // Mouse wheel scrolled.
    /* Context usage:
     * FlUInt8 zDelta = data.data.uint8[0];
     */
    FlEventCodeMouseWheel = 0x07,

    // Resize/resolution changed from the OS.
    /* Context usage:
     * FlUInt16 width = data.data.uint16[0];
     * FlUInt16 height = data.data.uint16[1];
     */
    FlEventCodeResized = 0x08,

    FlMaxEventCode = 0xFF
} FlSystemEventCode;
#endif // FL_CORE_EVENT_H
