// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.h

#pragma once

#ifndef FL_RENDERER_VULKAN_VULKANTYPES_INL
#define FL_RENDERER_VULKAN_VULKANTYPES_INL

#include "FlashlightEngine/Prerequisites.h"
#include "FlashlightEngine/Export.h"

#include <vulkan/vulkan.h>

typedef struct FlVulkanContext {
    VkInstance Instance;

    VkAllocationCallbacks* Allocator;
} FlVulkanContext;

#endif // FL_RENDERER_VULKAN_VULKANTYPES_INL
