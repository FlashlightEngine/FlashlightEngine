/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : SwapChain.hpp
 * Description : Declaration of the SwapChain class. The swap chain is used to store images that are being rendered.
 */
#pragma once

#include "FlashlightEngine/Renderer/VulkanWrapper/Device.hpp"

#include "FlashlightEngine/Core/Window.hpp"

namespace Flashlight::VulkanWrapper {
    class SwapChain {
        VkSwapchainKHR m_SwapChain = VK_NULL_HANDLE;
        std::vector<VkImage> m_SwapChainImages;
        VkFormat m_SwapChainImageFormat = VK_FORMAT_UNDEFINED;
        VkExtent2D m_SwapChainExtent;

        VkDevice m_Device = VK_NULL_HANDLE;
        VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
        QueueFamilyIndices m_QueueFamilies;
        SwapChainSupportDetails m_SwapChainSupport;
        GLFWwindow* m_Window = nullptr;
        
    public:
        inline SwapChain(const Device& device, const Window& window, const Surface& surface);
        inline ~SwapChain();

        SwapChain(const SwapChain&) = delete;
        SwapChain(SwapChain&&) = delete;

        SwapChain& operator=(const SwapChain&) = delete;
        SwapChain& operator=(SwapChain&&) = delete;

        [[nodiscard]] inline VkSwapchainKHR GetNativeSwapChain() const;
        [[nodiscard]] inline std::vector<VkImage> GetSwapChainImages() const;
        [[nodiscard]] inline VkFormat GetSwapChainImageFormat() const;
        [[nodiscard]] inline VkExtent2D GetSwapChainExtent() const;

    private:
        void Create();
        inline void Destroy() const;

        static VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
        static VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
        VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) const;
    };
 
#include "SwapChain.inl"
}
