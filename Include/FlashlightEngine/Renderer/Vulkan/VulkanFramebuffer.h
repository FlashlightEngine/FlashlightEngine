// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.h

#pragma once

#ifndef FL_RENDERER_VULKAN_VULKANFRAMEBUFFER_H
#define FL_RENDERER_VULKAN_VULKANFRAMEBUFFER_H

#include "FlashlightEngine/Prerequisites.h"
#include "FlashlightEngine/Export.h"

#include "FlashlightEngine/Renderer/Vulkan/VulkanTypes.inl"

void flVulkanFramebufferCreate(
    FlVulkanContext* context,
    FlVulkanRenderPass* renderPass,
    FlUInt32 width,
    FlUInt32 height,
    FlUInt32 attachmentCount,
    VkImageView* attachments,
    FlVulkanFramebuffer* outFramebuffer
);

void flVulkanFramebufferDestroy(FlVulkanContext* context, FlVulkanFramebuffer* framebuffer);

#endif // FL_RENDERER_VULKAN_VULKANFRAMEBUFFER_H
