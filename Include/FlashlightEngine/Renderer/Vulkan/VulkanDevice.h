// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.h

#pragma once

#ifndef FL_RENDERER_VULKAN_VULKANDEVICE_H
#define FL_RENDERER_VULKAN_VULKANDEVICE_H

#include "FlashlightEngine/Prerequisites.h"
#include "FlashlightEngine/Export.h"

#include "FlashlightEngine/Renderer/Vulkan/VulkanTypes.inl"

FlBool8 flVulkanDeviceCreate(FlVulkanContext* context);

void flVulkanDeviceDestroy(FlVulkanContext* context);

void flVulkanDeviceQuerySwapchainSupport(
    VkPhysicalDevice physicalDevice, 
    VkSurfaceKHR surface, 
    FlVulkanSwapchainSupport* outSupportInfo);

FlBool8 flVulkanDeviceDetectDepthFormat(FlVulkanDevice* device);

#endif // FL_RENDERER_VULKAN_VULKANDEVICE_H
