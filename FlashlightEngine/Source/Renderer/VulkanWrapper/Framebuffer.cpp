/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Framebuffer.cpp
 * Description : Definitions of methods from the Framebuffer class.
 */
#include "FlashlightEngine/Renderer/VulkanWrapper/Framebuffer.hpp"

namespace Flashlight::VulkanWrapper {
    Framebuffer::Framebuffer(const std::shared_ptr<Device>& device, const std::vector<VkImageView>& attachedImageViews,
                             const RenderPass& renderPass, const VkExtent2D extent) : m_Device(device) {
        VkFramebufferCreateInfo framebufferCreateInfo{};
        framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferCreateInfo.renderPass = renderPass.GetNativeRenderPass();
        framebufferCreateInfo.attachmentCount = static_cast<u32>(attachedImageViews.size());
        framebufferCreateInfo.pAttachments = attachedImageViews.data();
        framebufferCreateInfo.width = extent.width;
        framebufferCreateInfo.height = extent.height;
        framebufferCreateInfo.layers = 1;

        Log::EngineTrace("Creating Vulkan framebuffer...");
        if (vkCreateFramebuffer(m_Device->GetNativeDevice(), &framebufferCreateInfo, nullptr, &m_Framebuffer)
            != VK_SUCCESS) {
            Log::EngineFatal({0x01, 0x0A}, "Failed to create framebuffer.");
        } else {
            Log::EngineTrace("Vulkan framebuffer created.");
        }
    }
}
