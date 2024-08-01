/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : VulkanBufferUtils.hpp
 * Description : Abstraction and utilities for Vulkan pipelines.
 */
#pragma once

#include <FlashlightEngine/VulkanRenderer/VulkanTypes.hpp>

namespace Flashlight::VulkanRenderer::VulkanUtils {
    AllocatedBuffer CreateBuffer(VmaAllocator allocator, size allocSize, VkBufferUsageFlags usage,
                                 VmaMemoryUsage memoryUsage);
    void DestroyBuffer(VmaAllocator allocator, const AllocatedBuffer& buffer);
}
