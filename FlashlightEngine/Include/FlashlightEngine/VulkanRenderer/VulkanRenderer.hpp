/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : VulkanRenderer.hpp
 * Description : Declaration of the VulkanRenderer class. It is used to create, manage and destroy vulkan objects.
 */
#pragma once

#include <FlashlightEngine/VulkanRenderer/VulkanTypes.hpp>

#include <FlashlightEngine/Core/Window.hpp>

#include <VkBootstrap.h>

namespace Flashlight {
    enum class DebugLevel {
        None = 0,

        Errors = 1,
        Warnings = 2,
        Verbose = 3,
        Debug = 4
    };
    
    namespace VulkanRenderer {
        struct FrameData {
            VkCommandPool CommandPool;
            VkCommandBuffer MainCommandBuffer;

            VkSemaphore SwapchainSemaphore, RenderSemaphore;
            VkFence RenderFence;
        };

        constexpr u32 g_FrameOverlap = 2;
        
        class VulkanRenderer {
            VkInstance m_Instance = VK_NULL_HANDLE;                     // Vulkan library handle.
            VkDebugUtilsMessengerEXT m_DebugMessenger = VK_NULL_HANDLE; // Vulkan debug output handle.
            VkSurfaceKHR m_Surface;                                     // Vulkan window surface.
            VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;         // GPU chosen as the default device.
            VkDevice m_Device = VK_NULL_HANDLE;                         // Vulkan device for commands.

            VkSwapchainKHR m_Swapchain = VK_NULL_HANDLE;
            VkFormat m_SwapchainImageFormat = VK_FORMAT_UNDEFINED;

            std::vector<VkImage> m_SwapchainImages;
            std::vector<VkImageView> m_SwapchainImageViews;
            VkExtent2D m_SwapchainExtent;

            FrameData m_Frames[g_FrameOverlap];
            i32 m_FrameNumber = 0;

            FrameData& GetCurrentFrame() { return m_Frames[m_FrameNumber % g_FrameOverlap]; }

            VkQueue m_GraphicsQueue = VK_NULL_HANDLE;
            u32 m_GraphicsQueueFamily = 0;
            
        public:
            VulkanRenderer(const Window& window, const DebugLevel& debugLevel);
            ~VulkanRenderer();

            VulkanRenderer(const VulkanRenderer&) = delete;
            VulkanRenderer(VulkanRenderer&&) = delete;

            VulkanRenderer& operator=(const VulkanRenderer&) = delete;
            VulkanRenderer& operator=(VulkanRenderer&&) = delete;

            void BeginFrame();

        private:
            void InitializeVulkan(const Window& window, const DebugLevel& debugLevel);
            void InitializeSwapchain(const Window& window);
            void InitializeCommands();
            void InitializeSynchronisationPrimitives();

            void CreateSwapchain(u32 width, u32 height);
            void DestroySwapchain() const;
        };

    }
}
