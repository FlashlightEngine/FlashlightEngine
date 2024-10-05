// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.h

#pragma once

#ifndef FL_RENDERER_VULKAN_VULKANCOMMANDBUFFER_H
#define FL_RENDERER_VULKAN_VULKANCOMMANDBUFFER_H

#include "FlashlightEngine/Prerequisites.h"
#include "FlashlightEngine/Export.h"

#include "FlashlightEngine/Renderer/Vulkan/VulkanTypes.inl"

void flVulkanCommandBufferAllocate(
    FlVulkanContext* context,
    VkCommandPool pool,
    FlBool8 isPrimary,
    FlVulkanCommandBuffer* outCommandBuffer
);

void flVulkanCommandBufferFree(
    FlVulkanContext* context,
    VkCommandPool pool,
    FlVulkanCommandBuffer* commandBuffer
);

void flVulkanCommandBufferBegin(
    FlVulkanCommandBuffer* commandBuffer,
    FlBool8 isSingleUse,
    FlBool8 isRenderPassContinue,
    FlBool8 isSimultaneousUse
);

void flVulkanCommandBufferEnd(FlVulkanCommandBuffer* commandBuffer);

void flVulkanCommandBufferUpdateSubmitted(FlVulkanCommandBuffer* commandBuffer);

void flVulkanCommandBufferReset(FlVulkanCommandBuffer* commandBuffer);

/**
 * Allocates and begins recording to outCommandBuffer.
 */
void flVulkanCommandBufferAllocateAndBeginSingleUse(
    FlVulkanContext* context,
    VkCommandPool pool,
    FlVulkanCommandBuffer* outCommandBuffer
);

/**
 * Ends recording, submits and wait for queue operation and frees the provided commandBuffer.
 */
void flVulkanCommandBufferEndSingleUse(
    FlVulkanContext* context,
    VkCommandPool pool,
    FlVulkanCommandBuffer* commandBuffer,
    VkQueue queue
);

#endif // FL_RENDERER_VULKAN_VULKANCOMMANDBUFFER_H
