// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.h

#pragma once

#ifndef FL_RENDERER_VULKAN_VULKANRENDERPASS_H
#define FL_RENDERER_VULKAN_VULKANRENDERPASS_H

#include "FlashlightEngine/Prerequisites.h"
#include "FlashlightEngine/Export.h"

#include "FlashlightEngine/Renderer/Vulkan/VulkanTypes.inl"

void flVulkanRenderPassCreate(
    FlVulkanContext* context,
    FlVulkanRenderPass* outRenderPass,
    FlFloat32 x, FlFloat32 y, FlFloat32 w, FlFloat32 h,
    FlFloat32 r, FlFloat32 g, FlFloat32 b, FlFloat32 a,
    FlFloat32 depth,
    FlUInt32 stencil
);

void flVulkanRenderPassDestroy(FlVulkanContext* context, FlVulkanRenderPass* renderPass);

void flVulkanRenderPassBegin(
    FlVulkanCommandBuffer* commandBuffer,
    FlVulkanRenderPass* renderPass,
    VkFramebuffer framebuffer
);

void flVulkanRenderPassEnd(FlVulkanCommandBuffer* commandBuffer, FlVulkanRenderPass* renderPass);

#endif // FL_RENDERER_VULKAN_VULKANRENDERPASS_H
