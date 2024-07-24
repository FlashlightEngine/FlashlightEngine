/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : SwapChain.hpp
 * Description : Declaration of the SwapChain class. The swap chain is used to store images that are being rendered.
 */
#pragma once

#include "FlashlightEngine/Renderer/VulkanWrapper/Device.hpp"
#include "FlashlightEngine/Renderer/VulkanWrapper/RenderPass.hpp"
#include "FlashlightEngine/Renderer/VulkanWrapper/Framebuffer.hpp"

#include "FlashlightEngine/Core/Window.hpp"

namespace Flashlight {
    struct RenderObjects;

    namespace VulkanWrapper {
        class SwapChain {
            VkSwapchainKHR m_SwapChain = VK_NULL_HANDLE;
            std::vector<VkImage> m_SwapChainImages;
            std::vector<VkImageView> m_SwapChainImageViews;
            VkFormat m_SwapChainImageFormat = VK_FORMAT_UNDEFINED;
            VkExtent2D m_SwapChainExtent;
        
            std::unique_ptr<RenderPass> m_RenderPass;

            std::vector<Framebuffer> m_Framebuffers;

            Device& m_Device;
            VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
            QueueFamilyIndices m_QueueFamilies;
            SwapChainSupportDetails m_SwapChainSupport;
            GLFWwindow* m_Window = nullptr;

        public:
            SwapChain(Device& device, const Window& window, const Surface& surface);
            inline ~SwapChain();

            SwapChain(const SwapChain&) = delete;
            SwapChain(SwapChain&&) = delete;

            SwapChain& operator=(const SwapChain&) = delete;
            SwapChain& operator=(SwapChain&&) = delete;

            [[nodiscard]] inline VkSwapchainKHR GetNativeSwapChain() const;
            [[nodiscard]] inline std::vector<VkImage> GetSwapChainImages() const;
            [[nodiscard]] inline std::vector<VkImageView> GetSwapChainImageViews() const;
            [[nodiscard]] inline VkFormat GetSwapChainImageFormat() const;
            [[nodiscard]] inline VkExtent2D GetSwapChainExtent() const;

            [[nodiscard]] inline RenderPass& GetRenderPass() const;

            [[nodiscard]] inline VkFramebuffer GetFramebufferAtIndex(u32 index) const;

            void AcquireNextImageIndex(const RenderObjects& renderObjects, u32& imageIndex) const;

            void SubmitCommandBufferAndPresent(const RenderObjects& renderObjects, const u32 &imageIndex) const;

        private:
            void CreateSwapChain();
            void CreateSwapChainImageViews();
            void CreateRenderPass();
            void CreateFramebuffers();

            [[nodiscard]] static VkSurfaceFormatKHR ChooseSwapSurfaceFormat(
                const std::vector<VkSurfaceFormatKHR>& availableFormats);
            [[nodiscard]] static VkPresentModeKHR ChooseSwapPresentMode(
                const std::vector<VkPresentModeKHR>& availablePresentModes);
            [[nodiscard]] VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) const;
        };
    

#include "SwapChain.inl"
        
    }
}
