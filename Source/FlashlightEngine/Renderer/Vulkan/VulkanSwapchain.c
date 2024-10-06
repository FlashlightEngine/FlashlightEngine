// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.h

#include "FlashlightEngine/Renderer/Vulkan/VulkanSwapchain.h"

#include "FlashlightEngine/Renderer/Vulkan/VulkanDevice.h"
#include "FlashlightEngine/Renderer/Vulkan/VulkanImage.h"

#include "FlashlightEngine/Core/Logger.h"
#include "FlashlightEngine/Core/FlMemory.h"

void flVulkanSwapchainInternalCreate(FlVulkanContext* context, FlUInt32 width, FlUInt32 height, FlVulkanSwapchain* swapchain);
void flVulkanSwapchainInternalDestroy(FlVulkanContext* context, FlVulkanSwapchain* swapchain);

void flVulkanSwapchainCreate(
    FlVulkanContext* context,
    FlUInt32 width,
    FlUInt32 height,
    FlVulkanSwapchain* outSwapchain
) {
    // Simply create a new one.
    flVulkanSwapchainInternalCreate(context, width, height, outSwapchain);
}

void flVulkanSwapchainRecreate(
    FlVulkanContext* context,
    FlUInt32 width,
    FlUInt32 height,
    FlVulkanSwapchain* swapchain
) {
    // Destroy the old one and create a new one.
    flVulkanSwapchainInternalDestroy(context, swapchain);
    flVulkanSwapchainInternalCreate(context, width, height, swapchain);
}

void flVulkanSwapchainDestroy(
    FlVulkanContext* context,
    FlVulkanSwapchain* swapchain
) {
    flVulkanSwapchainInternalDestroy(context, swapchain);
}

FlBool8 flVulkanSwapchainAcquireNextImageIndex(
    FlVulkanContext* context,
    FlVulkanSwapchain* swapchain,
    FlUInt64 timeoutNs,
    VkSemaphore imageAvailableSemaphore,
    VkFence fence,
    FlUInt32* outImageIndex
) {
    VkResult result = vkAcquireNextImageKHR(
        context->Device.LogicalDevice,
        swapchain->Handle,
        timeoutNs,
        imageAvailableSemaphore,
        fence,
        outImageIndex
    );

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        // Trigger swapchain recreation, then boot out of the render loop.
        flVulkanSwapchainRecreate(context, context->FramebufferWidth, context->FramebufferHeight, swapchain);
        return FALSE;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        FL_LOG_FATAL("Failed to acquire swapchain image.")
        return FALSE;
    }

    return TRUE;
}

void flVulkanSwapchainPresent(
    FlVulkanContext* context,
    FlVulkanSwapchain* swapchain,
    VkQueue graphicsQueue,
    VkQueue presentQueue,
    VkSemaphore renderCompleteSemaphore,
    FlUInt32 presentImageIndex
) {
    VkPresentInfoKHR presentInfo = {VK_STRUCTURE_TYPE_PRESENT_INFO_KHR};
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &renderCompleteSemaphore;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &swapchain->Handle;
    presentInfo.pImageIndices = &presentImageIndex;
    presentInfo.pResults = 0;

    VkResult result = vkQueuePresentKHR(presentQueue, &presentInfo);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        // Swapchain is out of date, subobtimal or a framebuffer resize has occurred. Trigger swapchain recreation.
        flVulkanSwapchainRecreate(context, context->FramebufferWidth, context->FramebufferHeight, swapchain);
    } else if (result != VK_SUCCESS) {
        FL_LOG_FATAL("Failed to present swapchain image.")
    }

    context->CurrentFrame = (context->CurrentFrame + 1) % swapchain->MaxFramesInFlight;
}

void flVulkanSwapchainInternalCreate(FlVulkanContext* context, FlUInt32 width, FlUInt32 height, FlVulkanSwapchain* swapchain) {
    VkExtent2D swapchainExtent = {width, height};
    swapchain->MaxFramesInFlight = 2;

    // Choose a swapchain surface format.
    FlBool8 found = FALSE;
    for (FlUInt32 i = 0; i < context->Device.SwapchainSupport.FormatCount; ++i) {
        VkSurfaceFormatKHR format = context->Device.SwapchainSupport.Formats[i];
        // Preferred formats.
        if (format.format == VK_FORMAT_B8G8R8A8_UNORM &&
            format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            swapchain->ImageFormat = format;
            found = TRUE;
            break;
        }
    }

    if (!found) {
        swapchain->ImageFormat = context->Device.SwapchainSupport.Formats[0];
    }

    // Swapchain presentation mode.
    // FIFO is guaranteed to be available.
    VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
    for (FlUInt32 i = 0; i < context->Device.SwapchainSupport.PresentModeCount; ++i) {
        VkPresentModeKHR mode = context->Device.SwapchainSupport.PresentModes[i];
        if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
            presentMode = mode;
            break;
        }
    }

    // Requery swapchain support.
    flVulkanDeviceQuerySwapchainSupport(
        context->Device.PhysicalDevice,
        context->Surface,
        &context->Device.SwapchainSupport
    );

    // Swapchain extent.
    if (context->Device.SwapchainSupport.Capabilities.currentExtent.width != UINT32_MAX) {
        swapchainExtent = context->Device.SwapchainSupport.Capabilities.currentExtent;
    }

    // Clamp to the value allowed by the GPU.
    VkExtent2D min = context->Device.SwapchainSupport.Capabilities.minImageExtent;
    VkExtent2D max = context->Device.SwapchainSupport.Capabilities.maxImageExtent;
    swapchainExtent.width = FL_CLAMP(swapchainExtent.width, min.width, max.width);
    swapchainExtent.height = FL_CLAMP(swapchainExtent.height, min.height, max.height);

    FlUInt32 imageCount = context->Device.SwapchainSupport.Capabilities.minImageCount + 1;
    if (context->Device.SwapchainSupport.Capabilities.maxImageCount > 0 && imageCount > context->Device.SwapchainSupport.Capabilities.maxImageCount) {
        imageCount = context->Device.SwapchainSupport.Capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR swapchainCreateInfo = {VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR};
    swapchainCreateInfo.surface = context->Surface;
    swapchainCreateInfo.minImageCount = imageCount;
    swapchainCreateInfo.imageFormat = swapchain->ImageFormat.format;
    swapchainCreateInfo.imageColorSpace = swapchain->ImageFormat.colorSpace;
    swapchainCreateInfo.imageExtent = swapchainExtent;
    swapchainCreateInfo.imageArrayLayers = 1;
    swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    // Setup the queue family indices.
    if (context->Device.GraphicsQueueIndex != context->Device.PresentQueueIndex) {
        FlUInt32 queueFamilyIndices[] = {
            (FlUInt32)context->Device.GraphicsQueueIndex,
            (FlUInt32)context->Device.PresentQueueIndex
        };
        swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchainCreateInfo.queueFamilyIndexCount = 2;
        swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        // Ignored when sharing mode is set to exclusive.
        swapchainCreateInfo.queueFamilyIndexCount = 0;
        swapchainCreateInfo.pQueueFamilyIndices = 0;    
    }

    swapchainCreateInfo.preTransform = context->Device.SwapchainSupport.Capabilities.currentTransform;
    swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainCreateInfo.presentMode = presentMode;
    swapchainCreateInfo.clipped = VK_TRUE;
    swapchainCreateInfo.oldSwapchain = 0;

    VK_CHECK(vkCreateSwapchainKHR(context->Device.LogicalDevice, &swapchainCreateInfo, context->Allocator, &swapchain->Handle))

    context->CurrentFrame = 0;

    // Images
    swapchain->ImageCount = 0;
    VK_CHECK(vkGetSwapchainImagesKHR(context->Device.LogicalDevice, swapchain->Handle, &swapchain->ImageCount, 0))
    if (!swapchain->Images) {
        swapchain->Images = (VkImage*)flAllocate(sizeof(VkImage) * swapchain->ImageCount, FlMemoryTagRenderer);
    }
    if (!swapchain->ImageViews) {
        swapchain->ImageViews = (VkImageView*)flAllocate(sizeof(VkImageView) * swapchain->ImageCount, FlMemoryTagRenderer);
    }
    VK_CHECK(vkGetSwapchainImagesKHR(context->Device.LogicalDevice, swapchain->Handle, &swapchain->ImageCount, swapchain->Images))

    for (FlUInt32 i = 0; i < swapchain->ImageCount; ++i) {
        VkImageViewCreateInfo viewInfo = {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
        viewInfo.image = swapchain->Images[i];
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = swapchain->ImageFormat.format;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        VK_CHECK(vkCreateImageView(context->Device.LogicalDevice, &viewInfo, context->Allocator, &swapchain->ImageViews[i]))
    }

    // Depth resources
    if (!flVulkanDeviceDetectDepthFormat(&context->Device)) {
        context->Device.DepthFormat = VK_FORMAT_UNDEFINED;
        FL_LOG_FATAL("Failed to find a supported depth format.")
    }

    // Create depth image and its view.
    flVulkanImageCreate(
        context,
        VK_IMAGE_TYPE_2D,
        swapchainExtent.width,
        swapchainExtent.height,
        context->Device.DepthFormat,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        TRUE,
        VK_IMAGE_ASPECT_DEPTH_BIT,
        &swapchain->DepthAttachment
    );
    
    FL_LOG_DEBUG("Vulkan swapchain created successfully.")
}

void flVulkanSwapchainInternalDestroy(FlVulkanContext* context, FlVulkanSwapchain* swapchain) {
    flVulkanImageDestroy(context, &swapchain->DepthAttachment);

    // Only destroy the views, not the images, since those are owned by the swapchain and are thus
    // destroyed when it is.
    for (FlUInt32 i = 0; i < swapchain->ImageCount; ++i) {
        vkDestroyImageView(context->Device.LogicalDevice, swapchain->ImageViews[i], context->Allocator);
    }

    vkDestroySwapchainKHR(context->Device.LogicalDevice, swapchain->Handle, context->Allocator);
}

