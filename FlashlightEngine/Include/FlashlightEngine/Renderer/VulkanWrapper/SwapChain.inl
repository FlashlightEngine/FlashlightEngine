/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : SwapChain.inl
 * Description : Definitions of inline methods from the SwapChain class.
 */
#pragma once

inline SwapChain::~SwapChain() {    
    for (const auto imageView : m_SwapChainImageViews) {
        if (imageView) {
            Log::EngineTrace("Destroying Vulkan swap chain image view.");
            vkDestroyImageView(m_Device.GetNativeDevice(), imageView, nullptr);
        }
    }
    
    if (m_SwapChain) {
        Log::EngineTrace("Destroying Vulkan swap chain and images.");
        vkDestroySwapchainKHR(m_Device.GetNativeDevice(), m_SwapChain, nullptr);
    }
}

inline VkSwapchainKHR SwapChain::GetNativeSwapChain() const {
    if (m_SwapChain) {
        return m_SwapChain;
    }

    return VK_NULL_HANDLE;
}

inline std::vector<VkImage> SwapChain::GetSwapChainImages() const {
    return m_SwapChainImages;
}

inline std::vector<VkImageView> SwapChain::GetSwapChainImageViews() const {
    return m_SwapChainImageViews;
}

inline VkFormat SwapChain::GetSwapChainImageFormat() const {
    return m_SwapChainImageFormat;
}

inline VkExtent2D SwapChain::GetSwapChainExtent() const {
    return m_SwapChainExtent; 
}

inline RenderPass& SwapChain::GetRenderPass() const {
    return *m_RenderPass;
}

inline VkFramebuffer SwapChain::GetFramebufferAtIndex(const u32 index) const {
    return m_Framebuffers[index].GetNativeFramebuffer();
}
