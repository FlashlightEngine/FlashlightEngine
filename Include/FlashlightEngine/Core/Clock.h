// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.h

#pragma once

#ifndef FL_CORE_CLOCK_H
#define FL_CORE_CLOCK_H

#include "FlashlightEngine/Prerequisites.h"
#include "FlashlightEngine/Export.h"

typedef struct FlClock {
    FlFloat64 StartTime;
    FlFloat64 Elapsed;
} FlClock;

// Update the provided clock. Should be called just before checking elapsed time.
// Has no effect on non-started clocks.
void flClockUpdate(FlClock* clock);

// Starts the provided clock. Resets elapsed time.
void flClockStart(FlClock* clock);

// Stops the provided clock. Does not reset elapsed time.
void flClockStop(FlClock* clock);

#endif // FL_CORE_CLOCK_H
