/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : RenderPass.inl
 * Description : Definitions of inline methods from the RenderPass class.
 */
#pragma once

inline RenderPass::RenderPass(const VulkanWrapper::Device& device, const RenderPassInfo& renderPassInfo)
    : m_Device(device.GetNativeDevice()) {
    Create(renderPassInfo);
}

inline RenderPass::~RenderPass() {
    Destroy();
}

inline VkRenderPass RenderPass::GetNativeRenderPass() const {
    return m_RenderPass;
}

inline void RenderPass::Destroy() const {
    if (m_RenderPass) {
        vkDestroyRenderPass(m_Device, m_RenderPass, nullptr);
    }
}
