// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.h

#pragma once

#ifndef FL_RENDERER_RENDERTYPES_INL
#define FL_RENDERER_RENDERTYPES_INL

#include "FlashlightEngine/Prerequisites.h"
#include "FlashlightEngine/Export.h"

typedef enum FlRendererBackendType {
    FlRendererBackendVulkan,
    FlRendererBackendOpenGL,
    FlRendererBackendDirectX
} FlRendererBackendType;

typedef struct FlRendererBackend {
    struct FlPlatformState* PlatformState;

    FlUInt64 FrameNumber;

    FlBool8 (*Initialize)(struct FlRendererBackend* backend, const char* applicationName, struct FlPlatformState* platformState);

    void (*Shutdown)(struct FlRendererBackend* backend);

    void (*Resized)(struct FlRendererBackend* backend, FlUInt16 width, FlUInt16 height);

    FlBool8 (*BeginFrame)(struct FlRendererBackend* backend, FlFloat32 deltaTime);
    FlBool8 (*EndFrame)(struct FlRendererBackend* backend, FlFloat32 deltaTime);
} FlRendererBackend;

typedef struct FlRenderPacket {
    FlFloat32 DeltaTime;
} FlRenderPacket;

#endif // FL_RENDERER_RENDERTYPES_INL
