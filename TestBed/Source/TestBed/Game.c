// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.h

#include "TestBed/Game.h"

#include <FlashlightEngine/Core/Logger.h>

FlBool8 GameInitialize(FlGame* gameInstance) {
    FL_LOG_DEBUG("GameInitialize() called.");
    return TRUE;
}

FlBool8 GameUpdate(FlGame* gameInstance, FlFloat32 deltaTime) {
    return TRUE;
}

FlBool8 GameRender(FlGame* gameInstance, FlFloat32 deltaTime) {
    return TRUE;
}

void GameOnResize(FlGame* gameInstance, FlUInt32 width, FlUInt32 height) {
}
