// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.h

#pragma once

#ifndef FL_CORE_APPLICATION_H
#define FL_CORE_APPLICATION_H

#include "FlashlightEngine/Prerequisites.h"
#include "FlashlightEngine/Export.h"

struct FlGame;

typedef struct FlApplicationConfiguration {
    // Window starting position x axis, if applicable.
    FlInt16 StartPosX;

    // Window starting position y axis, if applicable.
    FlInt16 StartPosY;

    // Window starting width, if applicable.
    FlInt16 StartWidth;

    // Window starting height, if applicable.
    FlInt16 StartHeight;

    // The application name used for the window, if applicable.
    char* Name;
} FlApplicationConfiguration;

FL_API FlBool8 flApplicationCreate(struct FlGame* gameInstance);
FL_API FlBool8 flApplicationRun(void);

void flApplicationGetFramebufferSize(FlUInt32* width, FlUInt32* height);

#endif // FL_CORE_APPLICATION_H
