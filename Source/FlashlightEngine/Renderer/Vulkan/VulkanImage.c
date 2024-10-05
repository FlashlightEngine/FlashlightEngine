// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.h

#include "FlashlightEngine/Renderer/Vulkan/VulkanImage.h"

#include "FlashlightEngine/Renderer/Vulkan/VulkanDevice.h"

#include "FlashlightEngine/Core/FlMemory.h"
#include "FlashlightEngine/Core/Logger.h"

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
) {
    outImage->Width = width;
    outImage->Height = height;

    VkImageCreateInfo imageCreateInfo = {VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};
    imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    imageCreateInfo.extent.width = width;
    imageCreateInfo.extent.height = height;
    imageCreateInfo.extent.depth = 1; // TODO: Support configurable depth.
    imageCreateInfo.mipLevels = 4;    // TODO: Support mip mapping.
    imageCreateInfo.arrayLayers = 1;  // TODO: Support number of layers in the image.
    imageCreateInfo.format = format;
    imageCreateInfo.tiling = tiling;
    imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageCreateInfo.usage = usage;
    imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;         // TODO: Configurable sample count.
    imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // TODO: Configurable sharing mode.

    VK_CHECK(vkCreateImage(context->Device.LogicalDevice, &imageCreateInfo, context->Allocator, &outImage->Handle))

    VkMemoryRequirements memoryRequirements;
    vkGetImageMemoryRequirements(context->Device.LogicalDevice, outImage->Handle, &memoryRequirements);

    FlInt32 memoryType = context->FindMemoryIndex(memoryRequirements.memoryTypeBits, memoryFlags);
    if (memoryType == -1) {
        FL_LOG_ERROR("Required memory type not found. Image not valid.")
    }

    // Allocate memory.
    VkMemoryAllocateInfo allocInfo = {VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
    allocInfo.allocationSize = memoryRequirements.size;
    allocInfo.memoryTypeIndex = (FlUInt32)memoryType;
    VK_CHECK(vkAllocateMemory(context->Device.LogicalDevice, &allocInfo, context->Allocator, &outImage->Memory))

    // Bind the memory.
    VK_CHECK(vkBindImageMemory(context->Device.LogicalDevice, outImage->Handle, outImage->Memory, 0)) // TODO: Configurable memory offset

    if (createView) {
        outImage->ImageView = VK_NULL_HANDLE;
        flVulkanImageViewCreate(context, format, outImage, viewAspectFlags);
    }
}

void flVulkanImageViewCreate(
    FlVulkanContext* context,
    VkFormat format,
    FlVulkanImage* image,
    VkImageAspectFlags aspectFlags
) {
    VkImageViewCreateInfo viewCreateInfo = {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
    viewCreateInfo.image = image->Handle;
    viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D; // TODO: Make configurable.
    viewCreateInfo.format = format;
    viewCreateInfo.subresourceRange.aspectMask = aspectFlags;

    // TODO: Make configurable.
    viewCreateInfo.subresourceRange.baseMipLevel = 0;
    viewCreateInfo.subresourceRange.levelCount = 1;
    viewCreateInfo.subresourceRange.baseArrayLayer = 0;
    viewCreateInfo.subresourceRange.layerCount = 1;
    
    VK_CHECK(vkCreateImageView(context->Device.LogicalDevice, &viewCreateInfo, context->Allocator, &image->ImageView))
}

void flVulkanImageDestroy(FlVulkanContext* context, FlVulkanImage* image) {
    if (image->ImageView) {
        vkDestroyImageView(context->Device.LogicalDevice, image->ImageView, context->Allocator);
        image->ImageView = VK_NULL_HANDLE;
    }

    if (image->Memory) {
        vkFreeMemory(context->Device.LogicalDevice, image->Memory, context->Allocator);
        image->Memory = VK_NULL_HANDLE;
    }

    if (image->Handle) {
        vkDestroyImage(context->Device.LogicalDevice, image->Handle, context->Allocator);
        image->Handle = VK_NULL_HANDLE;
    }
}
