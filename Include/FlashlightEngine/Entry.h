// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.h

#pragma once

#ifndef FL_ENTRY_H
#define FL_ENTRY_H

#include "FlashlightEngine/Prerequisites.h"
#include "FlashlightEngine/Export.h"

#include "FlashlightEngine/GameTypes.h"

#include "FlashlightEngine/Core/Application.h"
#include "FlashlightEngine/Core/Logger.h"

// Externally defined function to create a game.
extern FlBool8 flCreateGame(FlGame* outGame);

/**
 * The main entry point of the application.
 */
int main(void) {
    // Request the game instance from the application.
    FlGame gameInstance;
    if (!flCreateGame(&gameInstance)) {
        FL_LOG_FATAL("Could not create game instance.");
        return -1;
    }

    // Ensure the function pointers exist.
    if (!gameInstance.Render || !gameInstance.Update || !gameInstance.Initialize || !gameInstance.OnResize) {
        FL_LOG_FATAL("The game's function pointers must be assigned.");
        return -2;
    }

    // Initialization.
    if (!flApplicationCreate(&gameInstance)) {
        FL_LOG_INFO("Application failed to create.");
        return 1;
    }
    
    // Begin the game loop.
    if (!flApplicationRun()) {
        FL_LOG_INFO("Application did not shutdown correctly.");
        return 2;
    }
    
    return 0;
}

#endif // FL_ENTRY_H
