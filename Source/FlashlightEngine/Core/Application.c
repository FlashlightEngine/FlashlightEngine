// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.h

#include "FlashlightEngine/Core/Application.h"

#include "FlashlightEngine/GameTypes.h"

#include "FlashlightEngine/Core/Logger.h"
#include "FlashlightEngine/Core/FlMemory.h"
#include "FlashlightEngine/Core/Event.h"

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

FlBool8 flApplicationCreate(FlGame* gameInstance) {
    if (Initialized) {
        FL_LOG_ERROR("flApplicationCreate called more than once.");
        return FALSE;
    }

    ApplicationState.GameInstance = gameInstance;

    // Initialize subsystems.
    flInitializeLogging();

    // TODO: Remove this when logging improved
    FL_LOG_FATAL("Fatal Log Test Message: %f", 3.14f)
    FL_LOG_ERROR("Error Log Test Message: %f", 3.14f)
    FL_LOG_WARN("Warn Log Test Message: %f", 3.14f)
    FL_LOG_INFO("Info Log Test Message: %f", 3.14f)
    FL_LOG_DEBUG("Debug Log Test Message: %f", 3.14f)
    FL_LOG_TRACE("Trace Log Test Message: %f", 3.14f)

    ApplicationState.IsRunning = TRUE;
    ApplicationState.IsSuspended = FALSE;

    if (!flEventInitialize()) {
        FL_LOG_ERROR("Event system initialization failed. Application cannot continue.")
        return FALSE;
    }

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
        }
    }

    ApplicationState.IsRunning = FALSE;
    
    flEventShutdown();

    flPlatformShutdown(&ApplicationState.Platform);

    return TRUE;
}
