// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.h

#pragma once

#ifndef FL_RENDERER_VULKAN_VULKANTYPES_INL
#define FL_RENDERER_VULKAN_VULKANTYPES_INL

#include "FlashlightEngine/Prerequisites.h"
#include "FlashlightEngine/Export.h"

#include "FlashlightEngine/Core/Asserts.h"

#include <vulkan/vulkan.h>

// Check the given expression's result return value against VK_SUCCESS.
#define VK_CHECK(expr)                                   \
    {                                                    \
        if ((expr) != VK_SUCCESS) {                      \
            FL_ASSERT_MSG(FALSE, "Vulkan call failed."); \
        }                                                \
    }

typedef struct FlVulkanSwapchainSupport {
    VkSurfaceCapabilitiesKHR Capabilities;
    FlUInt32 FormatCount;
    VkSurfaceFormatKHR* Formats;
    FlUInt32 PresentModeCount;
    VkPresentModeKHR* PresentModes;
} FlVulkanSwapchainSupport;

typedef struct FlVulkanDevice {
    VkPhysicalDevice PhysicalDevice;
    VkDevice LogicalDevice;
    FlVulkanSwapchainSupport SwapchainSupport;

    FlUInt32 GraphicsQueueIndex;
    FlUInt32 PresentQueueIndex;
    FlUInt32 TransferQueueIndex;

    VkQueue GraphicsQueue;
    VkQueue PresentQueue;
    VkQueue TransferQueue;

    VkPhysicalDeviceProperties Properties;
    VkPhysicalDeviceFeatures Features;
    VkPhysicalDeviceMemoryProperties MemoryProperties;
} FlVulkanDevice;

typedef struct FlVulkanContext {
    VkInstance Instance;

    VkAllocationCallbacks* Allocator;

    VkSurfaceKHR Surface;

#ifdef FL_DEBUG
    VkDebugUtilsMessengerEXT DebugMessenger;
#endif

    FlVulkanDevice Device;
} FlVulkanContext;

#endif // FL_RENDERER_VULKAN_VULKANTYPES_INL
