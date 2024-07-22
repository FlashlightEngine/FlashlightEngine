/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : SwapChain.inl
 * Description : Definitions of inline methods from the SwapChain class.
 */
#pragma once

inline SwapChain::SwapChain(const Device& device, const Window& window,
                            const Surface& surface) : m_Device(device.GetNativeDevice()),
                                                      m_Surface(surface.GetNativeSurface()),
                                                      m_QueueFamilies(device.GetQueueFamilies()),
                                                      m_SwapChainSupport(device.GetSwapChainSupport()),
                                                      m_Window(window.GetGlfwWindow()) {
    CreateSwapChain();
    CreateSwapChainImageViews();
}

inline SwapChain::~SwapChain() {    
    Destroy();
}

inline VkSwapchainKHR SwapChain::GetNativeSwapChain() const {
    return m_SwapChain;
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

inline void SwapChain::Destroy() const {
    for (const auto imageView : m_SwapChainImageViews) {
        if (imageView) {
            vkDestroyImageView(m_Device, imageView, nullptr);
        }
    }
    
    if (m_SwapChain) {
        Log::EngineTrace("Destroying Vulkan swap chain.");
        vkDestroySwapchainKHR(m_Device, m_SwapChain, nullptr);
    }
}
