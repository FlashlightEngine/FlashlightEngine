/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : VulkanRenderer.hpp
 * Description : Declaration of the VulkanRenderer class. It is used to create, manage and destroy vulkan objects.
 */
#pragma once

#include <FlashlightEngine/VulkanRenderer/VulkanTypes.hpp>
#include <FlashlightEngine/VulkanRenderer/VulkanDescriptors.hpp>
#include <FlashlightEngine/VulkanRenderer/VulkanUtils/VulkanPipelines.hpp>

#include <FlashlightEngine/Core/Window.hpp>

#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_vulkan.h>

#include <VkBootstrap.h>

namespace Flashlight::VulkanRenderer {

    constexpr u32 g_FrameOverlap = 2;

    class VulkanRenderer {
        bool m_RendererInitialized = false;
        
        VkInstance m_Instance = VK_NULL_HANDLE;                     // Vulkan library handle.
        VkDebugUtilsMessengerEXT m_DebugMessenger = VK_NULL_HANDLE; // Vulkan debug output handle.
        VkSurfaceKHR m_Surface = VK_NULL_HANDLE;                    // Vulkan window surface.
        VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;         // GPU chosen as the default device.
        VkDevice m_Device = VK_NULL_HANDLE;                         // Vulkan device for commands.
        VmaAllocator m_Allocator;

        VkQueue m_GraphicsQueue = VK_NULL_HANDLE;
        u32 m_GraphicsQueueFamily = 0;
        VkQueue m_PresentQueue = VK_NULL_HANDLE;
        u32 m_PresentQueueFamily = 0;

        VkSwapchainKHR m_Swapchain = VK_NULL_HANDLE;
        VkFormat m_SwapchainImageFormat = VK_FORMAT_UNDEFINED;

        std::vector<VkImage> m_SwapchainImages;
        std::vector<VkImageView> m_SwapchainImageViews;
        VkExtent2D m_SwapchainExtent = {0, 0};

        DeletionQueue m_MainDeletionQueue;

        FrameData m_Frames[g_FrameOverlap];
        i32 m_FrameNumber = 0;

        AllocatedImage m_DrawImage;
        VkExtent2D m_DrawExtent;

        DescriptorAllocator m_GlobalDescriptorAllocator;

        VkDescriptorSet m_DrawImageDescriptors;
        VkDescriptorSetLayout m_DrawImageDescriptorLayout;

        VkPipeline m_GradientPipeline;
        VkPipelineLayout m_GradientPipelineLayout;

        VkFence m_ImmediateFence;
        VkCommandBuffer m_ImmediateCommandBuffer;
        VkCommandPool m_ImmediateCommandPool;

    public:
        VulkanRenderer(const Window& window, const DebugLevel& debugLevel);
        ~VulkanRenderer();

        VulkanRenderer(const VulkanRenderer&) = delete;
        VulkanRenderer(VulkanRenderer&&) = delete;

        VulkanRenderer& operator=(const VulkanRenderer&) = delete;
        VulkanRenderer& operator=(VulkanRenderer&&) = delete;

        void Draw();

    private:
        void InitializeVulkan(const Window& window, const DebugLevel& debugLevel);
        void InitializeSwapchain(const Window& window);
        void InitializeCommands();
        void InitializeSynchronisationPrimitives();
        void InitializeDescriptors();
        void InitializePipelines();
        void InitializeBackgroundPipeline();
        void InitializeImGui(const Window& window);
        
        void DrawBackground(VkCommandBuffer commandBuffer) const;
        void DrawImGui(VkCommandBuffer commandBuffer, VkImageView targetImageView) const;

        FrameData& GetCurrentFrame() {
            return m_Frames[m_FrameNumber % g_FrameOverlap];
        }

        void ImmediateSubmit(const std::function<void(VkCommandBuffer commandBuffer)>& function) const;
    };

}
