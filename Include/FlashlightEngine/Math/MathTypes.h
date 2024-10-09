// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.h

#pragma once

#ifndef FL_MATHTYPES_H
#define FL_MATHTYPES_H

#include "FlashlightEngine/Prerequisites.h"
#include "FlashlightEngine/Export.h"

typedef union FlVec2U {
    // An array of x, y
    FlFloat32 Elements[2];
    struct {
        union {
            // The first element.
            FlFloat32 X, R, S, U;
        };

        union {
            // The second element.
            FlFloat32 Y, G, T, V;
        };
    };
} FlVec2;

typedef union FlVec3U {
    // An array of x, y, z
    FlFloat32 Elements[3];
    struct {
        union {
        // The first element.
            FlFloat32 X, R, S, U;
        };

        union {
            // The second element.
            FlFloat32 Y, G, T, V;
        };

        union {
            // The third element.
            FlFloat32 Z, B, P, W;
        };
    };
} FlVec3;

typedef union FlVec4U {
    // An array of x, y, z, w
    FlFloat32 Elements[4];
    
    union {
        struct {
            union {
                // The first element.
                FlFloat32 X, R, S;
            };

            union {
                // The second element.
                FlFloat32 Y, G, T;
            };

            union {
                // The third element.
                FlFloat32 Z, B, P;
            };

            union {
                // The fourth element.
                FlFloat32 W, A, Q;
            };
        };
    };
} FlVec4;

typedef FlVec4 FlQuat;

typedef union FlMat4U {
    FlFloat32 Data[16];
} FlMat4;

#endif // FL_MATHTYPES_H
