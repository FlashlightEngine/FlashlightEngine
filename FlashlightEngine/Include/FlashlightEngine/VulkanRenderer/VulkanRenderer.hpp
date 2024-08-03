/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : VulkanRenderer.hpp
 * Description : Declaration of the VulkanRenderer class. It is used to create, manage and destroy vulkan objects.
 */
#pragma once

#include <FlashlightEngine/VulkanRenderer/VulkanAssetsLoader.hpp>
#include <FlashlightEngine/VulkanRenderer/VulkanDescriptors.hpp>
#include <FlashlightEngine/VulkanRenderer/VulkanWrapper/Swapchain.hpp>

#include <FlashlightEngine/Core/Window.hpp>

#include <VkBootstrap.h>

namespace Flashlight::VulkanRenderer {

    constexpr u32 g_FrameOverlap = 2;
    
    struct FrameData {
        VkCommandPool CommandPool;
        VkCommandBuffer MainCommandBuffer;

        VkSemaphore SwapchainSemaphore, RenderSemaphore;
        VkFence RenderFence;

        u32 SwapchainImageIndex;

        DeletionQueue DeletionQueue;
        DescriptorAllocatorGrowable FrameDescriptors;
    };

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

        DeletionQueue m_MainDeletionQueue;

        std::unique_ptr<VulkanWrapper::Instance> m_Instance;
        std::unique_ptr<VulkanWrapper::Device> m_Device;

        VmaAllocator m_Allocator;

        std::unique_ptr<VulkanWrapper::Swapchain> m_Swapchain;
        bool m_SwapchainResizeRequired{false};

        FrameData m_Frames[g_FrameOverlap];
        i32 m_FrameNumber = 0;

        VkFence m_ImmediateFence;
        VkCommandBuffer m_ImmediateCommandBuffer;
        VkCommandPool m_ImmediateCommandPool;

        AllocatedImage m_DrawImage;
        AllocatedImage m_DepthImage;
        VkExtent2D m_DrawExtent;
        float m_RenderScale = 1.0f;

        DescriptorAllocator m_GlobalDescriptorAllocator;

        VkDescriptorSet m_DrawImageDescriptors;
        VkDescriptorSetLayout m_DrawImageDescriptorLayout;

        GPUSceneData m_SceneData;
        VkDescriptorSetLayout m_GpuSceneDataLayout;
        
        VkPipelineLayout m_ComputePipelineLayout;

        std::vector<ComputeEffect> m_BackgroundEffects;
        i32 m_CurrentBackgroundEffect{0};

        VkPipelineLayout m_MeshPipelineLayout;
        VkPipeline m_MeshPipeline;

        VkDescriptorSetLayout m_SingleImageDescriptorLayout;

        std::vector<std::shared_ptr<MeshAsset>> m_TestMeshes;
        i32 m_CurrentMeshIndex{0};
        
        AllocatedImage m_WhiteImage;
        AllocatedImage m_BlackImage;
        AllocatedImage m_GrayImage;
        AllocatedImage m_ErrorCheckerboardImage;

        VkSampler m_DefaultSamplerLinear;
        VkSampler m_DefaultSamplerNearest;

    public:
        VulkanRenderer(const Window& window, const DebugLevel& debugLevel);
        ~VulkanRenderer();

        VulkanRenderer(const VulkanRenderer&) = delete;
        VulkanRenderer(VulkanRenderer&&) = delete;

        VulkanRenderer& operator=(const VulkanRenderer&) = delete;
        VulkanRenderer& operator=(VulkanRenderer&&) = delete;

        [[nodiscard]] GPUMeshBuffers UploadMesh(std::span<u32> indices, std::span<Vertex> vertices) const;
        void PlanMeshDeletion(GPUMeshBuffers mesh);
        void CreateUi();
        void Draw(Window& window);

        void ImmediateSubmit(const std::function<void(VkCommandBuffer commandBuffer)>& function) const;

    private:
        void InitializeVulkan(const Window& window, const DebugLevel& debugLevel);
        void InitializeSwapchain(const Window& window);
        void InitializeCommands();
        void InitializeFramesCommandBuffers();
        void InitializeImmediateCommandBuffer();
        void InitializeSynchronisationPrimitives();
        void InitializeDescriptors();
        void InitializePipelines();
        void InitializeComputePipelines();
        void InitializeMeshPipeline();
        void InitializeImGui(const Window& window);
        void InitializeDefaultData();

        void DrawBackground(VkCommandBuffer commandBuffer) const;
        void DrawGeometry(VkCommandBuffer commandBuffer);
        void DrawImGui(VkCommandBuffer commandBuffer, VkImageView targetImageView) const;

        [[nodiscard]] FrameData& GetCurrentFrame() {
            return m_Frames[m_FrameNumber % g_FrameOverlap];
        }

        void RecreateSwapchain(Window& window);
    };

}
