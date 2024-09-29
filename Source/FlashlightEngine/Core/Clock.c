// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.h

#include "FlashlightEngine/Core/Clock.h"

#include "FlashlightEngine/Platform/Platform.h"

void flClockUpdate(FlClock* clock) {
    if (clock->StartTime != 0) {
        clock->Elapsed = flPlatformGetAbsoluteTime() - clock->StartTime;
    }
}

void flClockStart(FlClock* clock) {
    clock->StartTime = flPlatformGetAbsoluteTime();
    clock->Elapsed = 0;
}

void flClockStop(FlClock* clock) {
    clock->StartTime = 0;
}
