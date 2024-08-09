/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : VulkanBufferUtils.hpp
 * Description : Abstraction and utilities for Vulkan pipelines.
 */
#pragma once

#include <FlashlightEngine/VulkanRenderer/VulkanTypes.hpp>

VK_DEFINE_HANDLE(VmaAllocator)
VK_DEFINE_HANDLE(VmaAllocation)

namespace Flashlight::Renderer::VulkanUtils {
    struct AllocatedBuffer {
        VkBuffer Buffer;
        VmaAllocation Allocation;
    };

    enum class MemoryUsage : u32 {
        Unknown = 0,
        GpuOnly = 1,
        CpuOnly = 2,
        CpuToGpu = 3,
        GpuToCpu = 4,
        CpuCopy = 5,
        GpuLazilyAllocated = 6,
        Auto = 7,
        AutoPreferDevice = 8,
        AutoPreferHost = 9
    };
    
    AllocatedBuffer CreateBuffer(VmaAllocator allocator, size allocSize, VkBufferUsageFlags usage,
                                 MemoryUsage memoryUsage);
    void DestroyBuffer(VmaAllocator allocator, const AllocatedBuffer& buffer);
}
