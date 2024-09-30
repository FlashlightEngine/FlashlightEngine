// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.h

#pragma once

#ifndef FL_RENDERER_RENDERERFRONTEND_H
#define FL_RENDERER_RENDERERFRONTEND_H

#include "FlashlightEngine/Prerequisites.h"
#include "FlashlightEngine/Export.h"

#include "FlashlightEngine/Renderer/RenderTypes.inl"

struct FlStaticMeshData;
struct FlPlatformState;

FlBool8 flRendererInitialize(const char* applicationName, struct FlPlatformState* platformState);
void flRendererShutdown();

void flRendererOnResize(FlUInt16 width, FlUInt16 height);

FlBool8 flRendererDrawFrame(FlRenderPacket* packet);

#endif // FL_RENDERER_RENDERERFRONTEND_H
