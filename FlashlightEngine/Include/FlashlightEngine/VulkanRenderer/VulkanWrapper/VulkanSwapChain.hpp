/* Copyright (C) 2024 Jean "Pixfri" Letessier (jean.letessier@protonmail.com)
 * This file is part of "FlashLight Engine"
 * For conditions of distribution and use, see copyright notice in FlashLightEngine.hpp
 *
 * File : VulkanSwapChain.hpp
 * Description : This header file contains the definition of the VulkanSwapChain class, which is a wrapper for the Vulkan
 *               swap chain object.
 */

#pragma once

#include "FlashlightEngine/Core/Window.hpp"

#include "VulkanBase.hpp"
#include "VulkanDeviceObject.hpp"

#include <vulkan/vulkan.h>

namespace Flashlight {
namespace VulkanWrapper {
    /// @ingroup VulkanRenderer
    /// @struct Flashlight::VulkanWrapper::VulkanSwapChainObjects
    /// @brief Structures that stores Vulkan swap chain objects.
    struct VulkanSwapChainObjects {
        std::vector<VkImage> SwapChainImages;
        std::vector<VkImageView> SwapChainImageViews;
        VkFormat SwapChainImageFormat{};
        VkExtent2D SwapChainExtent{};

        /// @brief Constructor for the VulkanSwapChainObjects struct.
        /// Initializes every object to VK_NULL_HANDLE by default.
        VulkanSwapChainObjects() {}
    };

    
    class VulkanSwapChain final : public VulkanDeviceObject<VkSwapchainKHR> {
    public:
        inline VulkanSwapChain(Window &window, VulkanBase &base);

        // We can't copy/move vulkan objects.
        VulkanSwapChain(VulkanSwapChain &) = delete;
        VulkanSwapChain &operator=(VulkanSwapChain &) = delete;
        VulkanSwapChain(VulkanSwapChain &&) = delete;
        VulkanSwapChain &&operator=(VulkanSwapChain &&) = delete;

        inline static std::unique_ptr<VulkanSwapChain> Create(Window &window, VulkanBase &base);

    private:   
        VulkanSwapChainObjects m_SwapChainData;
    
        Window &m_Window;
        VulkanBase &m_VulkanBase;
        
        void Init() override;
        void Cleanup() const override;     

        // Functions to create/destroy vulkan objects.
        void CreateSwapChain();
        void DestroySwapChain() const;

        void CreateImageViews();
        void DestroyImageViews() const;

        // Util functions.
        VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats) const noexcept;
        VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes) const noexcept;
        VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities) const noexcept;
    };

#include "VulkanSwapChain.inl"

}

}
