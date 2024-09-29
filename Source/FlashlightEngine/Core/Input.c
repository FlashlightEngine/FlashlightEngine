// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.h

#include "FlashlightEngine/Core/Input.h"

#include "FlashlightEngine/Core/Event.h"
#include "FlashlightEngine/Core/FlMemory.h"
#include "FlashlightEngine/Core/Logger.h"

typedef struct FlKeyboardState {
    FlBool8 Keys[256];
} FlKeyboardState;

typedef struct FlMouseState {
    FlInt16 X;
    FlInt16 Y;
    FlUInt8 Buttons[FlButtonMaxButtons];
} FlMouseState;

typedef struct FlInputState {
    FlKeyboardState KeyboardCurrent;
    FlKeyboardState KeyboardPrevious;
    FlMouseState MouseCurrent;
    FlMouseState MousePrevious;
} FlInputState;

// Internal input state
static FlBool8 Initialized = FALSE;
static FlInputState State = {};

void flInputInitialize(void) {
    if (Initialized) {
        return;
    }

    flZeroMemory(&State, sizeof(FlInputState));
    Initialized = TRUE;
    FL_LOG_INFO("Input subsystem initialized.")
}

void flInputShutdown(void) {
    // TODO: Add shutdown routines when needed.
    Initialized = FALSE;
}

void flInputUpdate(FlFloat64 deltaTime) {
    FL_UNUSED(deltaTime);

    if (!Initialized) {
        return;
    }

    // Copy current states to previous states.
    flCopyMemory(&State.KeyboardPrevious, &State.KeyboardCurrent, sizeof(FlKeyboardState));
    flCopyMemory(&State.MousePrevious, &State.MouseCurrent, sizeof(FlMouseState));
}

FlBool8 flInputIsKeyDown(FlKeys key) {
    if (!Initialized) {
        return FALSE;
    }

    return State.KeyboardCurrent.Keys[key] == TRUE;
}

FlBool8 flInputIsKeyUp(FlKeys key) {
    if (!Initialized) {
        return TRUE;
    }

    return State.KeyboardCurrent.Keys[key] == FALSE;
}

FlBool8 flInputWasKeyDown(FlKeys key) {
    if (!Initialized) {
        return FALSE;
    }

    return State.KeyboardPrevious.Keys[key] == TRUE;
}

FlBool8 flInputWasKeyUp(FlKeys key) {
    if (!Initialized) {
        return TRUE;
    }

    return State.KeyboardPrevious.Keys[key] == FALSE;
}

void flInputProcessKey(FlKeys key, FlBool8 pressed) {
    // Only handle this if the state actually changed.
    if (State.KeyboardCurrent.Keys[key] != pressed) {
        // Update internal state.
        State.KeyboardCurrent.Keys[key] = pressed;

        // NOTE: Only for debug purpose.
        // FL_LOG_DEBUG("%c", (char)key)

        // Fire off an event for immediate processing.
        FlEventContext context;
        context.data.uint16[0] = key;
        flEventFire(pressed ? FlEventCodeKeyPressed : FlEventCodeKeyReleased, 0, context);
    }
}

FlBool8 flInputIsButtonDown(FlButtons button) {
    if (!Initialized) {
        return FALSE;
    }

    return State.MouseCurrent.Buttons[button] == TRUE;
}

FlBool8 flInputIsButtonUp(FlButtons button) {
    if (!Initialized) {
        return TRUE;
    }

    return State.MouseCurrent.Buttons[button] == FALSE;
}

FlBool8 flInputWasButtonDown(FlButtons button) {
    if (!Initialized) {
        return FALSE;
    }

    return State.MousePrevious.Buttons[button] == TRUE;
}

FlBool8 flInputWasButtonUp(FlButtons button) {
    if (!Initialized) {
        return TRUE;
    }

    return State.MousePrevious.Buttons[button] == FALSE;
}

void flInputGetMousePosition(FlInt32* x, FlInt32* y) {
    if (!Initialized) {
        *x = 0;
        *y = 0;
        return;
    }

    *x = State.MouseCurrent.X;
    *y = State.MouseCurrent.Y;
}

void flInputGetPreviousMousePosition(FlInt32* x, FlInt32* y) {
    if (!Initialized) {
        *x = 0;
        *y = 0;
        return;
    }

    *x = State.MousePrevious.X;
    *y = State.MousePrevious.Y;
}

void flInputProcessButton(FlButtons button, FlBool8 pressed) {
    // If the state changed, fire an event.
    if (State.MouseCurrent.Buttons[button] != pressed) {
        State.MouseCurrent.Buttons[button] = pressed;

        // Fire the event.
        FlEventContext context;
        context.data.uint16[0] = button;
        flEventFire(pressed ? FlEventCodeButtonPressed : FlEventCodeButtonReleased, 0, context);
    }
}

void flInputProcessMouseMove(FlInt16 x, FlInt16 y) {
    // If the state changed, fire an event.
    if (State.MouseCurrent.X != x || State.MouseCurrent.Y != y) {
        State.MouseCurrent.X = x;
        State.MouseCurrent.Y = y;

        // NOTE: Only for debug purpose.
        // FL_LOG_DEBUG("Mouse pos: (%i, %i)", x, y)

        // Fire the event.
        FlEventContext context;
        context.data.uint16[0] = x;
        context.data.uint16[1] = y;
        flEventFire(FlEventCodeMouseMoved, 0, context);
    }
}

void flInputProcessMouseWheel(FlInt8 zDelta) {
    // Fire the event.
    FlEventContext context;
    context.data.uint8[0] = zDelta;
    flEventFire(FlEventCodeMouseWheel, 0, context);
}
