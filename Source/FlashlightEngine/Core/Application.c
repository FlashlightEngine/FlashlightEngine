// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.h

#include "FlashlightEngine/Core/Application.h"

#include "FlashlightEngine/GameTypes.h"

#include "FlashlightEngine/Core/Logger.h"
#include "FlashlightEngine/Core/FlMemory.h"
#include "FlashlightEngine/Core/Event.h"
#include "FlashlightEngine/Core/Input.h"

#include "FlashlightEngine/Platform/Platform.h"

typedef struct FlApplicationState {
    FlGame* GameInstance;
    FlBool8 IsRunning;
    FlBool8 IsSuspended;
    FlPlatformState Platform;
    FlInt16 Width;
    FlInt16 Height;
    FlFloat64 LastTime;
} FlApplicationState;

static FlBool8 Initialized = FALSE;
static FlApplicationState ApplicationState;

// Event handlers
FlBool8 flApplicationOnEvent(FlUInt16 code, void* sender, void* listenerInstance, FlEventContext context);
FlBool8 flApplicationOnKey(FlUInt16 code, void* sender, void* listenerInstance, FlEventContext context);

FlBool8 flApplicationCreate(FlGame* gameInstance) {
    if (Initialized) {
        FL_LOG_ERROR("flApplicationCreate called more than once.");
        return FALSE;
    }

    ApplicationState.GameInstance = gameInstance;

    // Initialize subsystems.
    flLoggingInitialize();
    flInputInitialize();

    if (!flEventInitialize()) {
        FL_LOG_ERROR("Event system initialization failed. Application cannot continue.")
        return FALSE;
    }

    flEventRegister(FlEventCodeApplicationQuit, 0, flApplicationOnEvent);
    flEventRegister(FlEventCodeKeyPressed, 0, flApplicationOnKey);
    flEventRegister(FlEventCodeKeyReleased, 0, flApplicationOnKey);

    ApplicationState.IsRunning = TRUE;
    ApplicationState.IsSuspended = FALSE;

    if (!flPlatformStartup(
        &ApplicationState.Platform, 
        gameInstance->ApplicationConfiguration.Name, 
        gameInstance->ApplicationConfiguration.StartPosX, 
        gameInstance->ApplicationConfiguration.StartPosY,
        gameInstance->ApplicationConfiguration.StartWidth, 
        gameInstance->ApplicationConfiguration.StartHeight)) {
        return FALSE;
    }

    // Initialize the game.
    if (!ApplicationState.GameInstance->Initialize(ApplicationState.GameInstance)) {
        FL_LOG_FATAL("Game failed to initialize.");
        return FALSE;
    }

    ApplicationState.GameInstance->OnResize(ApplicationState.GameInstance, ApplicationState.Width, ApplicationState.Height);

    Initialized = TRUE;

    return TRUE;
}

FlBool8 flApplicationRun(void) {
    FL_LOG_INFO(flGetMemoryUsageString());
    
    while (ApplicationState.IsRunning) {
        if (!flPlatformPumpMessages(&ApplicationState.Platform)) {
            ApplicationState.IsRunning = FALSE;
        }

        if (!ApplicationState.IsSuspended) {
            if (!ApplicationState.GameInstance->Update(ApplicationState.GameInstance, (FlFloat32)0)) {
                FL_LOG_FATAL("Game update failed, shutting down.");
                ApplicationState.IsRunning = FALSE;
                break;
            }

            // Call the game's render routine.
            if (!ApplicationState.GameInstance->Render(ApplicationState.GameInstance, (FlFloat32)0)) {
                FL_LOG_FATAL("Game render failed, shutting down.");
                ApplicationState.IsRunning = FALSE;
                break;
            }

            // NOTE: Input update/state copying should always be handled
            // after any input should be recored; I.E. before this line.
            // As a safety, input is the last thing to be updated before
            // this frame ends.
            flInputUpdate(0);
        }
    }

    ApplicationState.IsRunning = FALSE;

    flPlatformShutdown(&ApplicationState.Platform);

    flEventUnregister(FlEventCodeApplicationQuit, 0, flApplicationOnEvent);
    flEventUnregister(FlEventCodeKeyPressed, 0, flApplicationOnKey);
    flEventUnregister(FlEventCodeKeyReleased, 0, flApplicationOnKey);
    
    flEventShutdown();

    flInputShutdown();

    flLoggingShutdown();

    return TRUE;
}

FlBool8 flApplicationOnEvent(FlUInt16 code, void* sender, void* listenerInstance, FlEventContext context) {
    switch (code) {
    case FlEventCodeApplicationQuit:
        {
            FL_LOG_INFO("FlEventCodeApplicationQuit recieved, shutting down.")
            ApplicationState.IsRunning = FALSE;
            return TRUE;
        }
    }

    return FALSE;
}

FlBool8 flApplicationOnKey(FlUInt16 code, void* sender, void* listenerInstance, FlEventContext context) {
    if (code == FlEventCodeKeyPressed) {
        FlUInt16 keyCode = context.data.uint16[0];
        if (keyCode == FlKeyEscape) {
            // NOTE: Technically firing an event to itself, but there may be other listeners.
            FlEventContext data = {};
            flEventFire(FlEventCodeApplicationQuit, 0, data);

            // Block anything else from processing this.
            return TRUE;
        } else if (keyCode == FlKeyA) {
            // Example on checking for a key
            FL_LOG_DEBUG("Explicit - A key pressed.")
        } else {
            FL_LOG_DEBUG("'%c' key pressed in window.", keyCode)
        }
    } else if (code == FlEventCodeKeyReleased) {
        FlUInt16 keyCode = context.data.uint16[0];
        if (keyCode == FlKeyB) {
            // Example on checking for a key
            FL_LOG_DEBUG("Explicit - B key released")
        } else {
            FL_LOG_DEBUG("'%c' key released in window.", keyCode)
        }
    }

    return FALSE;
}
