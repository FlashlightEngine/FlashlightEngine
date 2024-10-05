// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.h

#pragma once

#ifndef FL_RENDERER_VULKAN_VULKANIMAGE_H
#define FL_RENDERER_VULKAN_VULKANIMAGE_H

#include "FlashlightEngine/Prerequisites.h"
#include "FlashlightEngine/Export.h"

#include "FlashlightEngine/Renderer/Vulkan/VulkanTypes.inl"

void flVulkanImageCreate(
    FlVulkanContext* context,
    VkImageType imageType,
    FlUInt32 width,
    FlUInt32 height,
    VkFormat format,
    VkImageTiling tiling,
    VkImageUsageFlags usage,
    VkMemoryPropertyFlags memoryFlags,
    FlBool32 createView,
    VkImageAspectFlags viewAspectFlags,
    FlVulkanImage* outImage
);

void flVulkanImageViewCreate(
    FlVulkanContext* context,
    VkFormat format,
    FlVulkanImage* image,
    VkImageAspectFlags aspectFlags
);

void flVulkanImageDestroy(FlVulkanContext* context, FlVulkanImage* image);

#endif // FL_RENDERER_VULKAN_VULKANIMAGE_H
