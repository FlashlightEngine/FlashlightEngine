// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.h

#pragma once

#ifndef FL_RENDERER_VULKAN_VULKANBACKEND_H
#define FL_RENDERER_VULKAN_VULKANBACKEND_H

#include "FlashlightEngine/Prerequisites.h"
#include "FlashlightEngine/Export.h"

#include "FlashlightEngine/Renderer/RendererBackend.h"

FlBool8 flVulkanRendererBackendInitialize(FlRendererBackend* backend, const char* applicationName, struct FlPlatformState* platformState);
void flVulkanRendererBackendShutdown(FlRendererBackend* backend);

void flVulkanRendererBackendOnResize(FlRendererBackend* backend, FlUInt16 width, FlUInt16 height);

FlBool8 flVulkanRendererBackendBeginFrame(FlRendererBackend* backend, FlFloat32 deltaTime);
FlBool8 flVulkanRendererBackendEndFrame(FlRendererBackend* backend, FlFloat32 deltaTime);

#endif // FL_RENDERER_VULKAN_VULKANBACKEND_H
