// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.h

#pragma once

#ifndef FL_GAMETYPES_H
#define FL_GAMETYPES_H

#include "FlashlightEngine/Prerequisites.h"
#include "FlashlightEngine/Export.h"

#include "FlashlightEngine/Core/Application.h"

/**
 * Represents the basic game state in a game.
 * Called for creation by the application.
 */
typedef struct FlGame {
    // The application configuration.
    FlApplicationConfiguration ApplicationConfiguration;

    // Function pointer to the game's initialize function.
    FlBool8 (*Initialize)(struct FlGame* gameInstance);

    // Function pointer to the game's update function.
    FlBool8 (*Update)(struct FlGame* gameInstance, FlFloat32 deltaTime);

    // Function pointer to the game's render function.
    FlBool8 (*Render)(struct FlGame* gameInstance, FlFloat32 deltaTime);

    // Function pointer to handle resizes, if applicable.
    void (*OnResize)(struct FlGame* gameInstance, FlUInt32 width, FlUInt32 height);

    // Game-specific state. Created and managed by the game.
    void* State;
} FlGame;


#endif // FL_GAMETYPES_H
