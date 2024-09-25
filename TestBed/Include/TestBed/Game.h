// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.h

#pragma once

#ifndef FL_TESTBED_GAME_H
#define FL_TESTBED_GAME_H

#include <FlashlightEngine/Prerequisites.h>
#include <FlashlightEngine/GameTypes.h>

typedef struct GameState {
    FlFloat32 deltaTime;
} GameState;

FlBool8 GameInitialize(FlGame* gameInstance);

FlBool8 GameUpdate(FlGame* gameInstance, FlFloat32 deltaTime);

FlBool8 GameRender(FlGame* gameInstance, FlFloat32 deltaTime);

void GameOnResize(FlGame* gameInstance, FlUInt32 width, FlUInt32 height);

#endif // FL_TESTBED_GAME_H