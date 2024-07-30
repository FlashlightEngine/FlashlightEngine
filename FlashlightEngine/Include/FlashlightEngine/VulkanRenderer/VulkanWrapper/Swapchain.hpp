/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Swapchain.hpp
 * Description : Declaration of the Swapchain class. It is used to create a Vulkan swapchain.
 */
#pragma once

#include <FlashlightEngine/VulkanRenderer/VulkanTypes.hpp>

#include <FlashlightEngine/VulkanRenderer/VulkanWrapper/Device.hpp>

namespace Flashlight::VulkanRenderer::VulkanWrapper {    
    class Swapchain {
        VkSwapchainKHR m_Swapchain = VK_NULL_HANDLE;
        VkFormat m_SwapchainImageFormat = VK_FORMAT_UNDEFINED;

        std::vector<VkImage> m_SwapchainImages;
        std::vector<VkImageView> m_SwapchainImageViews;
        VkExtent2D m_SwapchainExtent = {0, 0};

        std::shared_ptr<Device> m_Device;
        
    public:
        Swapchain(const std::shared_ptr<Device>& device, const Surface& surface, const Window& window);
        ~Swapchain();

        Swapchain(const Swapchain&) = delete;
        Swapchain(Swapchain&& other) noexcept;

        Swapchain& operator=(const Swapchain&) = delete;
        Swapchain& operator=(Swapchain&& other) noexcept;

        [[nodiscard]] inline VkSwapchainKHR GetSwapchain() const;
        [[nodiscard]] inline VkFormat GetSwapchainImageFormat() const;
        [[nodiscard]] inline VkImage GetImageAtIndex(u32 index) const;
        [[nodiscard]] inline VkImageView GetImageViewAtIndex(u32 index) const;
        [[nodiscard]] inline VkExtent2D GetSwapchainExtent() const;

        VkResult AcquireNextSwapchainImage(const FrameData& frame, u32& imageIndex) const;
    };

#include "Swapchain.inl"
}
