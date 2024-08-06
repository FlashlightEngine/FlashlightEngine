/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Swapchain.hpp
 * Description : Declaration of the Device class. It is used to create and manage a swapchain. 
 */
#pragma once

#include <FlashlightEngine/VulkanRenderer/VulkanWrapper/Device.hpp>

namespace Flashlight::Renderer {
    struct FrameData;
    
    namespace VulkanWrapper {
        class Swapchain {
            VkSwapchainKHR m_Swapchain = VK_NULL_HANDLE;
            VkFormat m_SwapchainImageFormat = VK_FORMAT_UNDEFINED;
            vkb::Swapchain m_VkbSwapchain;

            std::vector<VkImage> m_SwapchainImages;
            std::vector<VkImageView> m_SwapchainImageViews;
            VkExtent2D m_SwapchainExtent = {0, 0};

            DeletionQueue m_DeletionQueue;

            bool m_Initialized = false;

        public:
            Swapchain(const Window& window, const Instance& instance, const Device& device);
            ~Swapchain();

            Swapchain(const Swapchain&) = delete;
            Swapchain(Swapchain&&) = delete;
        
            Swapchain& operator=(const Swapchain&) = delete;
            Swapchain& operator=(Swapchain&&) = delete;

            [[nodiscard]] inline VkSwapchainKHR GetSwapchain() const;
            [[nodiscard]] inline VkFormat GetSwapchainImageFormat() const;
            [[nodiscard]] inline vkb::Swapchain GetVkbSwapchain() const;
            [[nodiscard]] inline VkImage GetImageAtIndex(u32 index) const;
            [[nodiscard]] inline VkImageView GetImageViewAtIndex(u32 index) const;
            [[nodiscard]] inline VkExtent2D GetSwapchainExtent() const;

            [[nodiscard]] VkResult AcquireNextImage(const Device& device, FrameData& frame) const;

            void Destroy();
        };

#include <FlashlightEngine/VulkanRenderer/VulkanWrapper/Swapchain.inl>
    }
}