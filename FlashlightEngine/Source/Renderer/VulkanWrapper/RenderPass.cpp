/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : RenderPass.cpp
 * Description : Definitions of methods from the RenderPass class.
 */
#include "FlashlightEngine/Renderer/VulkanWrapper/RenderPass.hpp"

namespace Flashlight::VulkanWrapper {
    RenderPass::RenderPass(const std::shared_ptr<Device>& device, const RenderPassDescription& description) : m_Device(device) {
        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = static_cast<u32>(description.Attachments.size());
        renderPassInfo.pAttachments = description.Attachments.data();
        renderPassInfo.subpassCount = static_cast<u32>(description.Subpasses.size());
        renderPassInfo.pSubpasses = description.Subpasses.data();
        renderPassInfo.dependencyCount = static_cast<u32>(description.Dependencies.size());
        renderPassInfo.pDependencies = description.Dependencies.data();

        Log::EngineTrace("Creating Vulkan render pass...");
        if (vkCreateRenderPass(m_Device->GetNativeDevice(), &renderPassInfo, nullptr, &m_RenderPass) != VK_SUCCESS) {
            Log::EngineFatal({0x01, 0x09}, "Failed to create render pass.");
        } else {
            Log::EngineTrace("Vulkan render pass created.");
        }
    }
}
