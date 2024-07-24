/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Framebuffer.inl
 * Description : Definitions of inline methods from the Framebuffer class.
 */
#pragma once

inline Framebuffer::Framebuffer(const Device& device, const std::vector<VkImageView>& attachedImageViews,
                                const SwapChain& swapChain, const RenderPass& renderPass)
    : m_Device(device.GetNativeDevice()) {
    Create(renderPass.GetNativeRenderPass(), attachedImageViews, swapChain.GetSwapChainExtent());
}

inline Framebuffer::~Framebuffer() {
    Destroy();
}

inline VkFramebuffer Framebuffer::GetNativeFramebuffer() const {
    return m_Framebuffer;
}

inline void Framebuffer::Destroy() const {
    if (m_Framebuffer) {
        vkDestroyFramebuffer(m_Device, m_Framebuffer, nullptr);
    }
}
