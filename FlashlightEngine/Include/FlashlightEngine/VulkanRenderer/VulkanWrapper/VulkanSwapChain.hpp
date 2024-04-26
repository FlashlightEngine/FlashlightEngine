#pragma once

#include "VulkanDevice.hpp"
#include "VulkanWindowSurface.hpp"
#include "VulkanImageView.hpp"

#include "FlashlightEngine/Core/Window.hpp"

#include <volk.h>

namespace Flashlight {

namespace VulkanWrapper {

    class VulkanSwapChain {
    public:
        inline VulkanSwapChain();
        VulkanSwapChain(const VulkanSwapChain &swapChain) = delete;
        VulkanSwapChain(VulkanSwapChain &&swapChain) = delete;
        inline ~VulkanSwapChain();

        inline bool IsValid() const noexcept;
        inline VkSwapchainKHR GetHandle() const noexcept;

        inline std::vector<VkImage> GetSwapChainImages() const noexcept;
        inline VkImageView GetNativeImageViewAtIndex(i32 index) const noexcept;
        inline VkFormat GetSwapChainImageFormat() const noexcept;
        inline VkExtent2D GetSwapChainExtent() const noexcept;

        void Create(const VulkanDevice &device, const Window &window, const VulkanWindowSurface &windowSurface);
        inline void Destroy() const;

    private:
        VkDevice m_Device;
        VkSwapchainKHR m_Handle;
        std::vector<VkImage> m_SwapChainImages;
        std::vector<VulkanImageView> m_SwapChainImageViews;
        VkFormat m_SwapChainImageFormat;
        VkExtent2D m_SwapChainExtent;

        // Helper methods
        VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
        VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
        VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities, const Window &window);
    };

#include "VulkanSwapChain.inl"
    
}
    
}
