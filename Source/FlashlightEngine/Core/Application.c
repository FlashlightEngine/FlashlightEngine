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

static FlBool8 initialized = FALSE;
static FlApplicationState applicationState;

FlBool8 flApplicationCreate(FlGame* gameInstance) {
    if (initialized) {
        FL_LOG_ERROR("flApplicationCreate called more than once.");
        return FALSE;
    }

    applicationState.GameInstance = gameInstance;

    // Initialize subsystems.
    flInitializeLogging();

    // TODO: Remove this when logging improved
    FL_LOG_FATAL("Fatal Log Test Message: %f", 3.14f)
    FL_LOG_ERROR("Error Log Test Message: %f", 3.14f)
    FL_LOG_WARN("Warn Log Test Message: %f", 3.14f)
    FL_LOG_INFO("Info Log Test Message: %f", 3.14f)
    FL_LOG_DEBUG("Debug Log Test Message: %f", 3.14f)
    FL_LOG_TRACE("Trace Log Test Message: %f", 3.14f)

    applicationState.IsRunning = TRUE;
    applicationState.IsSuspended = FALSE;

    if (!flEventInitialize()) {
        FL_LOG_ERROR("Event system initialization failed. Application cannot continue.")
        return FALSE;
    }

    if (!flPlatformStartup(
        &applicationState.Platform, 
        gameInstance->ApplicationConfiguration.Name, 
        gameInstance->ApplicationConfiguration.StartPosX, 
        gameInstance->ApplicationConfiguration.StartPosY,
        gameInstance->ApplicationConfiguration.StartWidth, 
        gameInstance->ApplicationConfiguration.StartHeight)) {
        return FALSE;
    }

    // Initialize the game.
    if (!applicationState.GameInstance->Initialize(applicationState.GameInstance)) {
        FL_LOG_FATAL("Game failed to initialize.");
        return FALSE;
    }

    applicationState.GameInstance->OnResize(applicationState.GameInstance, applicationState.Width, applicationState.Height);

    initialized = TRUE;

    return TRUE;
}

FlBool8 flApplicationRun(void) {
    FL_LOG_INFO(flGetMemoryUsageString());
    
    while (applicationState.IsRunning) {
        if (!flPlatformPumpMessages(&applicationState.Platform)) {
            applicationState.IsRunning = FALSE;
        }

        if (!applicationState.IsSuspended) {
            if (!applicationState.GameInstance->Update(applicationState.GameInstance, (FlFloat32)0)) {
                FL_LOG_FATAL("Game update failed, shutting down.");
                applicationState.IsRunning = FALSE;
                break;
            }

            // Call the game's render routine.
            if (!applicationState.GameInstance->Render(applicationState.GameInstance, (FlFloat32)0)) {
                FL_LOG_FATAL("Game render failed, shutting down.");
                applicationState.IsRunning = FALSE;
                break;
            }
        }
    }

    applicationState.IsRunning = FALSE;
    
    flEventShutdown();

    flPlatformShutdown(&applicationState.Platform);

    return TRUE;
}
