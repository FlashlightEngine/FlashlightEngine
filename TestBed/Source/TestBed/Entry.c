// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.h

#include <FlashlightEngine/Entry.h>
// TODO: Remove this.
#include <FlashlightEngine/Platform/Platform.h>

#include "TestBed/Game.h"

// Define the function to create the game.
FlBool8 flCreateGame(FlGame* outGame) {
    // Application configuration.
    outGame->ApplicationConfiguration.StartPosX = 100;
    outGame->ApplicationConfiguration.StartPosY = 100;
    outGame->ApplicationConfiguration.StartWidth = 1280;
    outGame->ApplicationConfiguration.StartHeight = 720;
    outGame->ApplicationConfiguration.Name = "Flashlight Engine TestBed";

    // Define the game's function pointers.
    outGame->Initialize = GameInitialize;
    outGame->Update = GameUpdate;
    outGame->Render = GameRender;
    outGame->OnResize = GameOnResize;

    outGame->State = flPlatformAllocate(sizeof(GameState), FALSE);
    
    return TRUE;
}