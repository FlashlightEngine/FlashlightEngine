/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Swapchain.inl
 * Description : Definitions of inline methods from the Swapchain class.
 */
#pragma once

inline VkSwapchainKHR Swapchain::GetSwapchain() const {
    return m_Swapchain;
}

inline VkFormat Swapchain::GetSwapchainImageFormat() const {
    return m_SwapchainImageFormat;
}

inline vkb::Swapchain Swapchain::GetVkbSwapchain() const {
    return m_VkbSwapchain;
}

inline VkImage Swapchain::GetImageAtIndex(const u32 index) const {
    return m_SwapchainImages[index];
}

inline VkImageView Swapchain::GetImageViewAtIndex(const u32 index) const {
    return m_SwapchainImageViews[index];
}

inline VkExtent2D Swapchain::GetSwapchainExtent() const {
    return m_SwapchainExtent;
}
