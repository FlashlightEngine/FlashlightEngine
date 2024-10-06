// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.h

#pragma once

#ifndef FL_RENDERER_VULKAN_VULKANFENCE_H
#define FL_RENDERER_VULKAN_VULKANFENCE_H

#include "FlashlightEngine/Prerequisites.h"
#include "FlashlightEngine/Export.h"

#include "FlashlightEngine/Renderer/Vulkan/VulkanTypes.inl"

void flVulkanFenceCreate(
    FlVulkanContext* context,
    FlBool8 createSignaled,
    FlVulkanFence* outFence
);

void flVulkanFenceDestroy(FlVulkanContext* context, FlVulkanFence* fence);

FlBool8 flVulkanFenceWait(FlVulkanContext* context, FlVulkanFence* fence, FlUInt64 timeoutNs);

void flVulkanFenceReset(FlVulkanContext* context, FlVulkanFence* fence);

#endif // FL_RENDERER_VULKAN_VULKANFENCE_H
