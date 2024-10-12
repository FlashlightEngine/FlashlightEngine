// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.h

#include "FlashlightEngine/Core/Event.h"

#include "FlashlightEngine/Core/Logger.h"
#include "FlashlightEngine/Core/FlMemory.h"

#include "FlashlightEngine/Containers/DArray.h"

typedef struct FlRegisteredEvent {
    void* Listener;
    PFN_flOnEvent Callback;
} FlRegisteredEvent;

typedef struct FlEventCodeEntry {
    FlRegisteredEvent* Events;
} FlEventCodeEntry;

// This should be more than enough codes
#define FL_MAX_MESSAGE_CODES 16384

// State structure.
typedef struct FlEventSystemState {
    // Lookup table for event codes.
    FlEventCodeEntry Registered[FL_MAX_MESSAGE_CODES];
} FlEventSystemState;

/**
 * Event system internal state.
 */
static FlBool8 Initialized = false;
static FlEventSystemState State;

FlBool8 flEventInitialize(void) {
    if (Initialized) {
        return false;
    }

    flZeroMemory(&State, sizeof(State));
    Initialized = true;
    FL_LOG_INFO("Event subsystem initialized")

    return true;
}

void flEventShutdown(void) {
    // Free the events arrays. And objects pointed to should be destroyed on their own.
    for (FlUInt16 i = 0; i < FL_MAX_MESSAGE_CODES; ++i) {
        if (State.Registered[i].Events != 0) {
            flDArrayDestroy(State.Registered[i].Events);
            State.Registered[i].Events = 0;
        }
    }

    Initialized = false;
}

FlBool8 flEventRegister(FlUInt16 code, void* listener, PFN_flOnEvent onEvent) {
    if (Initialized == false) {
        return false;
    }

    if (State.Registered[code].Events == 0) {
        State.Registered[code].Events = flDArrayCreate(FlRegisteredEvent);
    }

    FlUInt64 registeredCount = flDArrayLength(State.Registered[code].Events);
    for (FlUInt64 i = 0; i < registeredCount; ++i) {
        if (State.Registered[code].Events[i].Listener == listener) {
            FL_LOG_WARN("Event listener already registered. Aborting registration.")
            return false;
        }
    }

    // If at this point, no duplicate was found, proceed with the registration.
    FlRegisteredEvent event;
    event.Listener = listener;
    event.Callback = onEvent;
    flDArrayPush(State.Registered[code].Events, event);

    return true;
}

FlBool8 flEventUnregister(FlUInt16 code, void* listener, PFN_flOnEvent onEvent) {
    if (Initialized == false) {
        return false;
    }

    // If nothing is registered for the code, boot out.
    if (State.Registered[code].Events == 0) {
        FL_LOG_WARN("The is no registered event for the code 0x%X. Aborting unregistration.", code)
        return false;
    }

    FlUInt64 registeredCount = flDArrayLength(State.Registered[code].Events);
    for (FlUInt64 i = 0; i < registeredCount; ++i) {
        FlRegisteredEvent e = State.Registered[code].Events[i];
        if (e.Listener == listener && e.Callback == onEvent) {
            // Found one, remove it.
            FlRegisteredEvent poppedEvent;
            flDArrayPopAt(State.Registered[code].Events, i, &poppedEvent);
            return true;
        }
    }

    // Not found.
    return false;
}

FlBool8 flEventFire(FlUInt16 code, void* sender, FlEventContext eventContext) {
    if (Initialized == false) {
        return false;
    }

    // If nothing is registered for the code, boot out.
    if (State.Registered[code].Events == 0) {
        return false;
    }

    FlUInt64 registeredCount = flDArrayLength(State.Registered[code].Events);
    for (FlUInt64 i = 0; i < registeredCount; ++i) {
        FlRegisteredEvent e = State.Registered[code].Events[i];
        if (e.Callback(code, sender, e.Listener, eventContext)) {
            // Message has been handled, do not send to any other listeneners.
            return true;
        }
    }

    // Not found.
    return false;
}
