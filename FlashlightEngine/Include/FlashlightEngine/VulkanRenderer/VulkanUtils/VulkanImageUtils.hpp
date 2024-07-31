/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : VulkanImageUtils.hpp
 * Description : Image related Vulkan-helpers.
 */
#pragma once

#include <FlashlightEngine/VulkanRenderer/VulkanTypes.hpp>

namespace Flashlight::VulkanRenderer::VulkanUtils {
    void TransitionImage(VkCommandBuffer commandBuffer, VkImage image, VkImageLayout currentLayout,
                         VkImageLayout newLayout);

    void CopyImageToImage(VkCommandBuffer commandBuffer, VkImage source, VkImage destination, VkExtent2D srcSize,
                          VkExtent2D dstSize);
}
