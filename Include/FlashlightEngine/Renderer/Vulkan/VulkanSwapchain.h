// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.h

#pragma once

#ifndef FL_RENDERER_VULKAN_VULKANSWAPCHAIN_H
#define FL_RENDERER_VULKAN_VULKANSWAPCHAIN_H

#include "FlashlightEngine/Prerequisites.h"
#include "FlashlightEngine/Export.h"

#include "FlashlightEngine/Renderer/Vulkan/VulkanTypes.inl"

void flVulkanSwapchainCreate(
    FlVulkanContext* context,
    FlUInt32 width,
    FlUInt32 height,
    FlVulkanSwapchain* outSwapchain
);

void flVulkanSwapchainRecreate(
    FlVulkanContext* context,
    FlUInt32 width,
    FlUInt32 height,
    FlVulkanSwapchain* swapchain
);

void flVulkanSwapchainDestroy(
    FlVulkanContext* context,
    FlVulkanSwapchain* swapchain
);

FlBool8 flVulkanSwapchainAcquireNextImageIndex(
    FlVulkanContext* context,
    FlVulkanSwapchain* swapchain,
    FlUInt64 timeoutNs,
    VkSemaphore imageAvailableSemaphore,
    VkFence fence,
    FlUInt32* outImageIndex
);

void flVulkanSwapchainPresent(
    FlVulkanContext* context,
    FlVulkanSwapchain* swapchain,
    VkQueue graphicsQueue,
    VkQueue presentQueue,
    VkSemaphore renderCompleteSemaphore,
    FlUInt32 presentImageIndex
);

#endif // FL_RENDERER_VULKAN_VULKANSWAPCHAIN_H
