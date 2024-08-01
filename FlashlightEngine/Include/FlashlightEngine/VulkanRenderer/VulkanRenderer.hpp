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
#include <FlashlightEngine/VulkanRenderer/VulkanWrapper/Instance.hpp>
#include <FlashlightEngine/VulkanRenderer/VulkanWrapper/Device.hpp>
#include <FlashlightEngine/VulkanRenderer/VulkanWrapper/Swapchain.hpp>

#include <FlashlightEngine/Core/Window.hpp>

#include <VkBootstrap.h>

namespace Flashlight::VulkanRenderer {

    constexpr u32 g_FrameOverlap = 2;

    struct ComputePushConstants {
        glm::vec4 Data1;
        glm::vec4 Data2;
        glm::vec4 Data3;
        glm::vec4 Data4;
    };

    struct ComputeEffect {
        const char* Name;

        VkPipeline Pipeline;
        VkPipelineLayout Layout;

        ComputePushConstants Data;
    };

    class VulkanRenderer {
        bool m_RendererInitialized = false;
        
        std::unique_ptr<VulkanWrapper::Instance> m_Instance;
        std::unique_ptr<VulkanWrapper::Device> m_Device;
        
        VmaAllocator m_Allocator;

        std::unique_ptr<VulkanWrapper::Swapchain> m_Swapchain;

        DeletionQueue m_MainDeletionQueue;

        FrameData m_Frames[g_FrameOverlap];
        i32 m_FrameNumber = 0;

        AllocatedImage m_DrawImage;
        VkExtent2D m_DrawExtent;

        DescriptorAllocator m_GlobalDescriptorAllocator;

        VkDescriptorSet m_DrawImageDescriptors;
        VkDescriptorSetLayout m_DrawImageDescriptorLayout;

        VkPipelineLayout m_ComputePipelineLayout;

        std::vector<ComputeEffect> m_BackgroundEffects;
        i32 m_CurrentBackgroundEffect{0};

        VkPipelineLayout m_TrianglePipelineLayout;
        VkPipeline m_TrianglePipeline;

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

        void CreateUi();
        void Draw();

    private:
        void InitializeVulkan(const Window& window, const DebugLevel& debugLevel);
        void InitializeSwapchain(const Window& window);
        void InitializeCommands();
        void InitializeSynchronisationPrimitives();
        void InitializeDescriptors();
        void InitializePipelines();
        void InitializeComputePipelines();
        void InitializeTrianglePipeline();
        void InitializeImGui(const Window& window);
        
        void DrawBackground(VkCommandBuffer commandBuffer) const;
        void DrawGeometry(VkCommandBuffer commandBuffer) const;
        void DrawImGui(VkCommandBuffer commandBuffer, VkImageView targetImageView) const;

        [[nodiscard]] FrameData& GetCurrentFrame() {
            return m_Frames[m_FrameNumber % g_FrameOverlap];
        }

        void ImmediateSubmit(const std::function<void(VkCommandBuffer commandBuffer)>& function) const;
    };

}
