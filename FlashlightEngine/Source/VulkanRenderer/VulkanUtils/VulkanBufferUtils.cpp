/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : VulkanBufferUtils.cpp
 * Description : Definitions of methods from VulkanBufferUtils.hpp.
 */
#include <FlashlightEngine/VulkanRenderer/VulkanUtils/VulkanBufferUtils.hpp>

namespace Flashlight::VulkanRenderer::VulkanUtils {
    AllocatedBuffer CreateBuffer(const VmaAllocator allocator, const size allocSize, const VkBufferUsageFlags usage,
                                 const VmaMemoryUsage memoryUsage) {
        // Allocate buffer
        VkBufferCreateInfo bufferInfo = {.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
        bufferInfo.pNext = nullptr;
        bufferInfo.size = allocSize;

        bufferInfo.usage = usage;

        VmaAllocationCreateInfo vmaAllocInfo{};
        vmaAllocInfo.usage = memoryUsage;
        vmaAllocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
        AllocatedBuffer newBuffer;

        VK_CHECK(
            vmaCreateBuffer(allocator, &bufferInfo, &vmaAllocInfo, &newBuffer.Buffer, &newBuffer.Allocation, &
                newBuffer.Info))

        return newBuffer;
    }

    void DestroyBuffer(const VmaAllocator allocator, const AllocatedBuffer& buffer) {
        vmaDestroyBuffer(allocator, buffer.Buffer, buffer.Allocation);
    }
}
