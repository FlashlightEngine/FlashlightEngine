// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.h

#pragma once

#ifndef FL_RENDERER_RENDERERBACKEND_H
#define FL_RENDERER_RENDERERBACKEND_H

#include "FlashlightEngine/Prerequisites.h"
#include "FlashlightEngine/Export.h"

#include "FlashlightEngine/Renderer/RenderTypes.inl"

struct FlPlatformState;

FlBool8 flRendererBackendCreate(FlRendererBackendType type, struct FlPlatformState* platformState, FlRendererBackend* outRenderBackend);
void flRendererBackendDestroy(FlRendererBackend* rendererBackend);

#endif // FL_RENDERER_RENDERERBACKEND_H
