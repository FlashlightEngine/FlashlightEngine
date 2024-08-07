/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : VulkanImageUtils.hpp
 * Description : Image related Vulkan-helpers.
 */
#pragma once

#include <FlashlightEngine/VulkanRenderer/VulkanTypes.hpp>

namespace Flashlight::Renderer {
    class VulkanRenderer;

    namespace VulkanUtils {
        void TransitionImage(VkCommandBuffer commandBuffer, VkImage image, VkImageLayout currentLayout,
                             VkImageLayout newLayout);

        void CopyImageToImage(VkCommandBuffer commandBuffer, VkImage source, VkImage destination,
                              VkExtent2D srcSize,
                              VkExtent2D dstSize);

        AllocatedImage CreateImage(VmaAllocator allocator, VkDevice device, VkExtent3D size, VkFormat format,
                                   VkImageUsageFlags usage, bool mipmapped = false);
        AllocatedImage CreateImage(VmaAllocator allocator, VkDevice device, const VulkanRenderer* renderer, const void* data,
                                   VkExtent3D size, VkFormat format, VkImageUsageFlags usage, bool mipmapped = false);
        void DestroyImage(VmaAllocator allocator, VkDevice device, const AllocatedImage& image);
        void GenerateMipmaps(VkCommandBuffer commandBuffer, VkImage image, VkExtent2D imageSize);
    }
}
