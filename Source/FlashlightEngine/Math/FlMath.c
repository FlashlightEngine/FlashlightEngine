// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.h

#include "FlashlightEngine/Math/FlMath.h"

#include "FlashlightEngine/Platform/Platform.h"

#include <math.h>
#include <stdlib.h>

static FlBool8 RandSeeded = false;

/*
 * NOTE: These are here in order to prevent including
 *       then entire <math.h> header everywhere. 
 */

FlFloat32 flSin(FlFloat32 x) {
    return sinf(x);
}

FlFloat32 flCos(FlFloat32 x) {
    return cosf(x);
}

FlFloat32 flTan(FlFloat32 x) {
    return tanf(x);
}

FlFloat32 flACos(FlFloat32 x) {
    return acosf(x);
}

FlFloat32 flSqrt(FlFloat32 x) {
    return sqrtf(x);
}

FlFloat32 flAbs(FlFloat32 x) {
    return fabsf(x);
}

FlInt32 flRandom() {
    if (!RandSeeded) {
        srand((FlUInt32)flPlatformGetAbsoluteTime());
        RandSeeded = true;
    }
    return rand();
}

FlInt32 flRandomInRange(FlInt32 min, FlInt32 max) {
    if (!RandSeeded) {
        srand((FlUInt32)flPlatformGetAbsoluteTime());
        RandSeeded = true;
    }

    return (rand() % (max - min + 1)) + min;
}

FlFloat32 flFRandom() {
    return (FlFloat32)flRandom() / (FlFloat32)RAND_MAX;
}

FlFloat32 flFRandomInRange(FlFloat32 min, FlFloat32 max) {
    return min + ((FlFloat32)flRandom() / ((FlFloat32)RAND_MAX / (max - min)));
}
