/* Copyright (C) 2024 Jean "Pixfri" Letessier (jean.letessier@protonmail.com)
 * This file is part of "FlashLight Engine"
 * For conditions of distribution and use, see copyright notice in FlashLightEngine.hpp
 *
 * VulkanSwapChain.hpp - The definition of the VulkanSwapChain class.
 * This header file contains the definition of the VulkanSwapChain class, which is a wrapper for the Vulkan swap chain object.
 */

#pragma once

#include "VulkanBase.hpp"
#include "FlashlightEngine/Core/Window.hpp"

#include <volk.h>

namespace Flashlight
{

class VulkanSwapChain {
public:
    inline VulkanSwapChain(Window &window, VulkanBase &base);
    inline ~VulkanSwapChain();

    // We can't copy/move vulkan objects.
    VulkanSwapChain(VulkanSwapChain &) = delete;
    VulkanSwapChain &operator=(VulkanSwapChain &) = delete;
    VulkanSwapChain(VulkanSwapChain &&) = delete;
    VulkanSwapChain &&operator=(VulkanSwapChain &&) = delete;

    inline static std::unique_ptr<VulkanSwapChain> Create(Window &window, VulkanBase &base);

private:
    VkSwapchainKHR m_SwapChain;
    
    Window &m_Window;
    VulkanBase &m_VulkanBase;
    
    void Init();
    void Cleanup() const;

    // Functions to create/destroy vulkan objects.
    void CreateSwapChain();
    void DestroySwapChain() const;

    // Util functions.
    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats) const noexcept;
    VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes) const noexcept;
    VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities) const noexcept;
};

#include "VulkanSwapChain.inl"

}
