// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.h

#include "FlashlightEngine/Renderer/Vulkan/VulkanCommandBuffer.h"

#include "FlashlightEngine/Core/FlMemory.h"

void flVulkanCommandBufferAllocate(
    FlVulkanContext* context,
    VkCommandPool pool,
    FlBool8 isPrimary,
    FlVulkanCommandBuffer* outCommandBuffer
) {
    flZeroMemory(outCommandBuffer, sizeof(FlVulkanCommandBuffer));

    VkCommandBufferAllocateInfo allocInfo = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    allocInfo.commandPool = pool;
    allocInfo.level = isPrimary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
    allocInfo.commandBufferCount = 1;
    allocInfo.pNext = 0;

    outCommandBuffer->State = FlVulkanCommandBufferStateNotAllocated;
    VK_CHECK(vkAllocateCommandBuffers(
        context->Device.LogicalDevice,
        &allocInfo,
        &outCommandBuffer->Handle
    ))
    outCommandBuffer->State = FlVulkanCommandBufferStateReady;
}

void flVulkanCommandBufferFree(
    FlVulkanContext* context,
    VkCommandPool pool,
    FlVulkanCommandBuffer* commandBuffer
) {
    vkFreeCommandBuffers(
        context->Device.LogicalDevice,
        pool,
        1,
        &commandBuffer->Handle
    );

    commandBuffer->Handle = VK_NULL_HANDLE;
    commandBuffer->State = FlVulkanCommandBufferStateNotAllocated;
}

void flVulkanCommandBufferBegin(
    FlVulkanCommandBuffer* commandBuffer,
    FlBool8 isSingleUse,
    FlBool8 isRenderPassContinue,
    FlBool8 isSimultaneousUse
) {
    VkCommandBufferBeginInfo beginInfo = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
    beginInfo.flags = 0;

    if (isSingleUse) {
        beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    }

    if (isRenderPassContinue) {
        beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
    }

    if (isSimultaneousUse) {
        beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
    }

    VK_CHECK(vkBeginCommandBuffer(commandBuffer->Handle, &beginInfo))
    commandBuffer->State = FlVulkanCommandBufferStateRecording;
}

void flVulkanCommandBufferEnd(FlVulkanCommandBuffer* commandBuffer) {
    VK_CHECK(vkEndCommandBuffer(commandBuffer->Handle))
    commandBuffer->State = FlVulkanCommandBufferStateRecordingEnded;
}

void flVulkanCommandBufferUpdateSubmitted(FlVulkanCommandBuffer* commandBuffer) {
    commandBuffer->State = FlVulkanCommandBufferStateSubmitted;
}

void flVulkanCommandBufferReset(FlVulkanCommandBuffer* commandBuffer) {
    commandBuffer->State = FlVulkanCommandBufferStateReady;
}

void flVulkanCommandBufferAllocateAndBeginSingleUse(
    FlVulkanContext* context,
    VkCommandPool pool,
    FlVulkanCommandBuffer* outCommandBuffer
) {
    flVulkanCommandBufferAllocate(context, pool, true, outCommandBuffer);
    flVulkanCommandBufferBegin(outCommandBuffer, true, false, false);
}

void flVulkanCommandBufferEndSingleUse(
    FlVulkanContext* context,
    VkCommandPool pool,
    FlVulkanCommandBuffer* commandBuffer,
    VkQueue queue
) {
    flVulkanCommandBufferEnd(commandBuffer);

    // Submit to the queue.
    VkSubmitInfo submitInfo = {VK_STRUCTURE_TYPE_SUBMIT_INFO};
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer->Handle;
    VK_CHECK(vkQueueSubmit(queue, 1, &submitInfo, 0))

    // Wait for it to finish.
    VK_CHECK(vkQueueWaitIdle(queue))

    flVulkanCommandBufferFree(context, pool, commandBuffer);
}
