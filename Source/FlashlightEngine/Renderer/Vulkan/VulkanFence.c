// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.h

#include "FlashlightEngine/Renderer/Vulkan/VulkanFence.h"

#include "FlashlightEngine/Core/Logger.h"

void flVulkanFenceCreate(
    FlVulkanContext* context,
    FlBool8 createSignaled,
    FlVulkanFence* outFence
) {
    outFence->Signaled = createSignaled;

    VkFenceCreateInfo fenceInfo = {VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
    fenceInfo.flags = (createSignaled) ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

    VK_CHECK(vkCreateFence(context->Device.LogicalDevice, &fenceInfo, context->Allocator, &outFence->Handle))
}

void flVulkanFenceDestroy(FlVulkanContext* context, FlVulkanFence* fence) {
    if (fence->Handle) {
        vkDestroyFence(context->Device.LogicalDevice, fence->Handle, context->Allocator);
        fence->Handle = VK_NULL_HANDLE;
    }
    fence->Signaled = false;
}

FlBool8 flVulkanFenceWait(FlVulkanContext* context, FlVulkanFence* fence, FlUInt64 timeoutNs) {
    if (fence->Signaled) {
        // If already signaled, do not wait.
        return true;
    }

    VkResult result = vkWaitForFences(context->Device.LogicalDevice, 1, &fence->Handle, true, timeoutNs);
    
    switch (result) {
    case VK_SUCCESS:
        fence->Signaled = true;
        return true;
    case VK_TIMEOUT:
        FL_LOG_WARN("flVulkanFenceWait - Timed Out")
        break;
    case VK_ERROR_DEVICE_LOST:
        FL_LOG_ERROR("flVulkanFenceWait - Device Lost")
        break;
    case VK_ERROR_OUT_OF_HOST_MEMORY:
        FL_LOG_ERROR("flVulkanFenceWait - Out Of Host Memory")
        break;
    case VK_ERROR_OUT_OF_DEVICE_MEMORY:
        FL_LOG_ERROR("flVulkanFenceWait - Out Of Device Memory")
        break;
    default:
        FL_LOG_ERROR("flVulkanFenceWait - An unkown error occured.")
        break;
    }

    return false;
}

void flVulkanFenceReset(FlVulkanContext* context, FlVulkanFence* fence) {
    if (fence->Signaled) {
        VK_CHECK(vkResetFences(context->Device.LogicalDevice, 1, &fence->Handle))
        fence->Signaled = false;
    }
}