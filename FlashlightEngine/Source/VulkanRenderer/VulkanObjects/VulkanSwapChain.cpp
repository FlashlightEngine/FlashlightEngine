/* Copyright (C) 2024 Jean "Pixfri" Letessier (jean.letessier@protonmail.com)
 * This file is part of "FlashLight Engine"
 * For conditions of distribution and use, see copyright notice in FlashLightEngine.hpp
 *
 * VulkanSwapChain.cpp - Definitions of methods from the VulkanSwapChain class.
 * This file contains the definitions of methods from the Flashlight::VulkanSwapChain class.
 */

#include "FlashlightEngine/VulkanRenderer/VulkanObjects/VulkanSwapChain.hpp"

#include "FlashlightEngine/defines.hpp"

#include <GLFW/glfw3.h>
#include <volk.h>

// Standard headers
#include <algorithm>
#include <limits>

/// @ingroup VulkanRenderer
/// @class Flashlight::VulkanSwapChain
/// @brief VulkanRenderer wrapper class for the Vulkan swap chain.

namespace Flashlight
{

/// @brief Initializes every Vulkan object this class is a wrapper of.
void VulkanSwapChain::Init() {
    CreateSwapChain();
}

/// @brief Destroys every Vulkan object this class is a wrapper of.
void VulkanSwapChain::Cleanup() const {
    DestroySwapChain();
}

/// @brief Create the VkSwapchainKHR object.
void VulkanSwapChain::CreateSwapChain() {
    SwapChainSupportDetails swapChainSupport = m_VulkanBase.GetSwapChainSupportDetails();

    VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.Formats);
    VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.PresentModes);
    VkExtent2D extent = ChooseSwapExtent(swapChainSupport.Capabilities);

    u32 imageCount = swapChainSupport.Capabilities.minImageCount + 1;

    // If there is a max image count and the imageCount variable exceeds it, clamp it to the max image count.
    if (swapChainSupport.Capabilities.maxImageCount > 0 && imageCount > swapChainSupport.Capabilities.maxImageCount) {
        imageCount = swapChainSupport.Capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = m_VulkanBase.Surface();

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = m_VulkanBase.GetQueueFamilies();
    u32 queueFamilyIndices[] = {indices.GraphicsFamily, indices.PresentFamily};

    if (indices.GraphicsFamily != indices.PresentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
    }

    createInfo.preTransform = swapChainSupport.Capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    createInfo.oldSwapchain = nullptr;

    if (vkCreateSwapchainKHR(m_VulkanBase.Device(), &createInfo, nullptr, &m_Vulkan.SwapChain) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create swap chain.");
    }

    vkGetSwapchainImagesKHR(m_VulkanBase.Device(), m_Vulkan.SwapChain, &imageCount, nullptr);
    m_Vulkan.SwapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(m_VulkanBase.Device(), m_Vulkan.SwapChain, &imageCount, m_Vulkan.SwapChainImages.data());
}

/// @brief Destroys the VkSwapchainKHR object.
void VulkanSwapChain::DestroySwapChain() const {
    vkDestroySwapchainKHR(m_VulkanBase.Device(), m_Vulkan.SwapChain, nullptr);    
}

/// @brief Chooses the best format to use for the swap chain.
///
/// @param availableFormats A std::vector of every available surface formats.
///
/// @returns If found, returns the preferred format for the swap chain, but if not found, returns the first format
/// available.
VkSurfaceFormatKHR VulkanSwapChain::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats) const noexcept {
    for (const auto &availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }

    return availableFormats[0];
}
/// @brief Chooses the best present mode to use for the swap chain.
///
/// @param availablePresentModes A std::vector of every present mode available.
///
/// @returns If available, returns VK_PRESENT_MODE_MAILBOX_KHR, if not, returns VK_PRESENT_MODE_FIFO_KHR.
VkPresentModeKHR VulkanSwapChain::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes) const noexcept {
    for (const auto &availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

/// @brief Chooses the best resolution for the swap chain.
///
/// @param capabilities The capabilities of the window surface.
///
/// @returns The resolution of the swap chain.
VkExtent2D VulkanSwapChain::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities) const noexcept {
    if (capabilities.currentExtent.width != std::numeric_limits<u32>::max()) {
        return capabilities.currentExtent;
    } else {
        int width, height;
        glfwGetFramebufferSize(m_Window.GetNativeWindow(), &width, &height);

        VkExtent2D actualExtent = {
            static_cast<u32>(width),
            static_cast<u32>(height)
        };

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.width = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}   

}
