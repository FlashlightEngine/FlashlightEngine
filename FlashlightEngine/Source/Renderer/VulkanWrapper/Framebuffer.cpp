/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Framebuffer.cpp
 * Description : Definitions of methods from the Framebuffer class.
 */
#include "FlashlightEngine/Renderer/VulkanWrapper/Framebuffer.hpp"

namespace Flashlight::VulkanWrapper {
    void Framebuffer::Create(const VkRenderPass renderPass, const std::vector<VkImageView>& attachedImageViews,
                             const VkExtent2D extent) {
        VkFramebufferCreateInfo framebufferCreateInfo{};
        framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        framebufferCreateInfo.renderPass = renderPass;
        framebufferCreateInfo.attachmentCount = static_cast<u32>(attachedImageViews.size());
        framebufferCreateInfo.pAttachments = attachedImageViews.data();
        framebufferCreateInfo.width = extent.width;
        framebufferCreateInfo.height = extent.height;
        framebufferCreateInfo.layers = 1;

        if (vkCreateFramebuffer(m_Device, &framebufferCreateInfo, nullptr, &m_Framebuffer) != VK_SUCCESS) {
            Log::EngineError("Failed to create framebuffer.");
        }
    }

}
