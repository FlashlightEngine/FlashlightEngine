/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : VulkanImageUtils.cpp
 * Description : Definitions of methods from VulkanImageUtils.hpp.
 */

#include <vk_mem_alloc.h>
#include <FlashlightEngine/VulkanRenderer/VulkanUtils/VulkanImageUtils.hpp>

#include <FlashlightEngine/VulkanRenderer/VulkanInitializers.hpp>
#include <FlashlightEngine/VulkanRenderer/VulkanRenderer.hpp>
#include <FlashlightEngine/VulkanRenderer/VulkanUtils/VulkanBufferUtils.hpp>


namespace Flashlight::Renderer::VulkanUtils {
    void TransitionImage(const VkCommandBuffer commandBuffer, const VkImage image,
                         const VkImageLayout currentLayout,
                         const VkImageLayout newLayout) {
        VkImageMemoryBarrier2 imageBarrier = {.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2};
        imageBarrier.pNext = nullptr;

        imageBarrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
        imageBarrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
        imageBarrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
        imageBarrier.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;

        imageBarrier.oldLayout = currentLayout;
        imageBarrier.newLayout = newLayout;

        const VkImageAspectFlags aspectMask = (newLayout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL)
                                                  ? VK_IMAGE_ASPECT_DEPTH_BIT
                                                  : VK_IMAGE_ASPECT_COLOR_BIT;

        imageBarrier.subresourceRange = VulkanInit::ImageSubresourceRange(aspectMask);
        imageBarrier.image = image;

        VkDependencyInfo depInfo{};
        depInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
        depInfo.pNext = nullptr;

        depInfo.imageMemoryBarrierCount = 1;
        depInfo.pImageMemoryBarriers = &imageBarrier;

        vkCmdPipelineBarrier2(commandBuffer, &depInfo);
    }

    void CopyImageToImage(const VkCommandBuffer commandBuffer, const VkImage source, const VkImage destination,
                          const VkExtent2D srcSize, const VkExtent2D dstSize) {
        VkImageBlit2 blitRegion{.sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2, .pNext = nullptr};

        blitRegion.srcOffsets[1].x = static_cast<i32>(srcSize.width);
        blitRegion.srcOffsets[1].y = static_cast<i32>(srcSize.height);
        blitRegion.srcOffsets[1].z = 1;

        blitRegion.dstOffsets[1].x = static_cast<i32>(dstSize.width);
        blitRegion.dstOffsets[1].y = static_cast<i32>(dstSize.height);
        blitRegion.dstOffsets[1].z = 1;

        blitRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blitRegion.srcSubresource.baseArrayLayer = 0;
        blitRegion.srcSubresource.layerCount = 1;
        blitRegion.srcSubresource.mipLevel = 0;

        blitRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blitRegion.dstSubresource.baseArrayLayer = 0;
        blitRegion.dstSubresource.layerCount = 1;
        blitRegion.dstSubresource.mipLevel = 0;

        VkBlitImageInfo2 blitInfo{.sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2, .pNext = nullptr};
        blitInfo.dstImage = destination;
        blitInfo.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        blitInfo.srcImage = source;
        blitInfo.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        blitInfo.filter = VK_FILTER_LINEAR;
        blitInfo.regionCount = 1;
        blitInfo.pRegions = &blitRegion;

        vkCmdBlitImage2(commandBuffer, &blitInfo);
    }

    AllocatedImage CreateImage(const VmaAllocator allocator, const VkDevice device, const VkExtent3D size,
                               const VkFormat format, const VkImageUsageFlags usage, const bool mipmapped) {
        AllocatedImage newImage;
        newImage.ImageFormat = format;
        newImage.ImageExtent = size;

        VkImageCreateInfo imageInfo = VulkanInit::ImageCreateInfo(format, usage, size);
        if (mipmapped) {
            imageInfo.mipLevels = static_cast<u32>(std::floor(std::log2(std::max(size.width, size.height)))) + 1;
        }

        // Always allocate images on dedicated GPU memory.
        VmaAllocationCreateInfo allocationInfo{};
        allocationInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
        allocationInfo.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

        VK_CHECK(
            vmaCreateImage(allocator, &imageInfo, &allocationInfo, &newImage.Image, &newImage.Allocation, nullptr))

        // If the format is a depth format, we will need to have it use the correct aspect flag.
        VkImageAspectFlags aspectFlag = VK_IMAGE_ASPECT_COLOR_BIT;
        if (format == VK_FORMAT_D32_SFLOAT) {
            aspectFlag = VK_IMAGE_ASPECT_DEPTH_BIT;
        }

        // Build an image view for the image.
        VkImageViewCreateInfo imageViewInfo = VulkanInit::ImageViewCreateInfo(format, newImage.Image, aspectFlag);
        imageViewInfo.subresourceRange.levelCount = imageInfo.mipLevels;

        VK_CHECK(vkCreateImageView(device, &imageViewInfo, nullptr, &newImage.ImageView))

        return newImage;
    }

    AllocatedImage CreateImage(const VmaAllocator allocator, const VkDevice device, const VulkanRenderer* renderer,
                               const void* data, const VkExtent3D size, const VkFormat format,
                               const VkImageUsageFlags usage, const bool mipmapped) {
        const std::size_t dataSize = static_cast<std::size_t>(size.depth * size.width * size.height) * 4;
        
        const AllocatedBuffer uploadBuffer = CreateBuffer(allocator, dataSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                                          MemoryUsage::CpuToGpu);

        VmaAllocationInfo uploadBufferAllocInfo;
        vmaGetAllocationInfo(allocator, uploadBuffer.Allocation, &uploadBufferAllocInfo);
        
        memcpy(uploadBufferAllocInfo.pMappedData, data, dataSize);

        const AllocatedImage newImage = CreateImage(allocator, device, size, format,
                                                    usage | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                                                    VK_IMAGE_USAGE_TRANSFER_SRC_BIT, mipmapped);

        renderer->ImmediateSubmit([&](const VkCommandBuffer commandBuffer) {
            TransitionImage(commandBuffer, newImage.Image, VK_IMAGE_LAYOUT_UNDEFINED,
                            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

            VkBufferImageCopy copyRegion{};
            copyRegion.bufferOffset = 0;
            copyRegion.bufferRowLength = 0;
            copyRegion.bufferImageHeight = 0;

            copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            copyRegion.imageSubresource.mipLevel = 0;
            copyRegion.imageSubresource.baseArrayLayer = 0;
            copyRegion.imageSubresource.layerCount = 1;
            copyRegion.imageExtent = size;

            // Copy the buffer into the image.
            vkCmdCopyBufferToImage(commandBuffer, uploadBuffer.Buffer, newImage.Image,
                                   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);

            if (mipmapped) {
                GenerateMipmaps(commandBuffer, newImage.Image, VkExtent2D{
                                    newImage.ImageExtent.width, newImage.ImageExtent.height
                                });
            } else {
                TransitionImage(commandBuffer, newImage.Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
            }
        });

        DestroyBuffer(allocator, uploadBuffer);

        return newImage;
    }

    void DestroyImage(const VmaAllocator allocator, const VkDevice device, const AllocatedImage& image) {
        vkDestroyImageView(device, image.ImageView, nullptr);
        vmaDestroyImage(allocator, image.Image, image.Allocation);
    }

    void GenerateMipmaps(const VkCommandBuffer commandBuffer, const VkImage image, VkExtent2D imageSize) {
        const i32 mipLevels = static_cast<i32>(std::floor(std::log2(std::max(imageSize.width, imageSize.height)))) +
            1;
        for (int mip = 0; mip < mipLevels; mip++) {
            VkExtent2D halfSize = imageSize;
            halfSize.width /= 2;
            halfSize.height /= 2;

            VkImageMemoryBarrier2 imageBarrier{.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2, .pNext = nullptr};

            imageBarrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
            imageBarrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
            imageBarrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
            imageBarrier.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;

            imageBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            imageBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

            constexpr VkImageAspectFlags aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            imageBarrier.subresourceRange = VulkanInit::ImageSubresourceRange(aspectMask);
            imageBarrier.subresourceRange.levelCount = 1;
            imageBarrier.subresourceRange.baseMipLevel = mip;
            imageBarrier.image = image;

            VkDependencyInfo depInfo{.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO, .pNext = nullptr};
            depInfo.imageMemoryBarrierCount = 1;
            depInfo.pImageMemoryBarriers = &imageBarrier;

            vkCmdPipelineBarrier2(commandBuffer, &depInfo);

            if (mip < mipLevels - 1) {
                VkImageBlit2 blitRegion{.sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2, .pNext = nullptr};

                blitRegion.srcOffsets[1].x = static_cast<i32>(imageSize.width);
                blitRegion.srcOffsets[1].y = static_cast<i32>(imageSize.height);
                blitRegion.srcOffsets[1].z = 1;

                blitRegion.dstOffsets[1].x = static_cast<i32>(halfSize.width);
                blitRegion.dstOffsets[1].y = static_cast<i32>(halfSize.height);
                blitRegion.dstOffsets[1].z = 1;

                blitRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                blitRegion.srcSubresource.baseArrayLayer = 0;
                blitRegion.srcSubresource.layerCount = 1;
                blitRegion.srcSubresource.mipLevel = mip;

                blitRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                blitRegion.dstSubresource.baseArrayLayer = 0;
                blitRegion.dstSubresource.layerCount = 1;
                blitRegion.dstSubresource.mipLevel = mip + 1;

                VkBlitImageInfo2 blitInfo{.sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2, .pNext = nullptr};
                blitInfo.dstImage = image;
                blitInfo.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                blitInfo.srcImage = image;
                blitInfo.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
                blitInfo.filter = VK_FILTER_LINEAR;
                blitInfo.regionCount = 1;
                blitInfo.pRegions = &blitRegion;

                vkCmdBlitImage2(commandBuffer, &blitInfo);

                imageSize = halfSize;
            }
        }

        // Transition all mip levels into the final read only layout
        TransitionImage(commandBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }
}
