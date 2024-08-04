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

    struct GLTFMetallic_Roughness {
        MaterialPipeline OpaquePipeline;
        MaterialPipeline TransparentPipeline;

        VkDescriptorSetLayout MaterialLayout;

        struct MaterialConstants {
            glm::vec4 ColorFactors;
            glm::vec4 MetalRoughFactors;
            // Padding, we need it anyway for uniform buffers.
            glm::vec4 Padding[14];
        };

        struct MaterialResources {
            AllocatedImage ColorImage;
            VkSampler ColorSampler;
            AllocatedImage MetalRoughnessImage;
            VkSampler MetalRoughnessSampler;
            VkBuffer DataBuffer;
            u32 DataBufferOffset;
        };

        DescriptorWriter Writer;

        void BuildPipelines(VulkanRenderer* renderer);
        void ClearResources(VkDevice device);

        MaterialInstance WriteMaterial(VkDevice device, MaterialPass pass, const MaterialResources& resources,
                                       DescriptorAllocatorGrowable& descriptorAllocator);
    };

    struct RenderObject {
        u32 IndexCount;
        u32 FirstIndex;
        VkBuffer IndexBuffer;

        MaterialInstance* Material;

        glm::mat4 Transform;
        VkDeviceAddress VertexBufferAddress;
    };

    struct DrawContext {
        std::vector<RenderObject> OpaqueSurfaces;
    };

    struct MeshNode : public Node {
        std::shared_ptr<MeshAsset> Mesh;

        void Draw(const glm::mat4& topMatrix, DrawContext& context) override;
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

        DescriptorAllocatorGrowable m_GlobalDescriptorAllocator;

        VkDescriptorSet m_DrawImageDescriptors;
        VkDescriptorSetLayout m_DrawImageDescriptorLayout;

        VkPipelineLayout m_ComputePipelineLayout;

        std::vector<ComputeEffect> m_BackgroundEffects;
        i32 m_CurrentBackgroundEffect{0};

        VkDescriptorSetLayout m_SingleImageDescriptorLayout;

        std::vector<std::shared_ptr<MeshAsset>> m_TestMeshes;

        AllocatedImage m_WhiteImage;
        AllocatedImage m_BlackImage;
        AllocatedImage m_GrayImage;
        AllocatedImage m_ErrorCheckerboardImage;

        VkSampler m_DefaultSamplerLinear;
        VkSampler m_DefaultSamplerNearest;

        MaterialInstance m_DefaultData;
        GLTFMetallic_Roughness m_MetalRoughMaterial;

    public:
        GPUSceneData SceneData;
        VkDescriptorSetLayout GpuSceneDataLayout;

        DrawContext MainDrawContext;
        std::unordered_map<std::string, std::shared_ptr<Node>> LoadedNodes;
        
        VulkanRenderer(const Window& window, const DebugLevel& debugLevel);
        ~VulkanRenderer();

        VulkanRenderer(const VulkanRenderer&) = delete;
        VulkanRenderer(VulkanRenderer&&) = delete;

        VulkanRenderer& operator=(const VulkanRenderer&) = delete;
        VulkanRenderer& operator=(VulkanRenderer&&) = delete;

        [[nodiscard]] GPUMeshBuffers UploadMesh(std::span<u32> indices, std::span<Vertex> vertices) const;
        void PlanMeshDeletion(GPUMeshBuffers mesh);
        void AddDeletion(std::function<void()>&& deletor);
        void CreateUi();
        void UpdateScene(const Window& window, Camera& camera);
        void Draw(Window& window, Camera& camera);

        void ImmediateSubmit(const std::function<void(VkCommandBuffer commandBuffer)>& function) const;

        [[nodiscard]] inline VulkanWrapper::Instance& GetInstance() const;
        [[nodiscard]] inline VulkanWrapper::Device& GetDevice() const;
        [[nodiscard]] inline VkFormat GetDrawImageFormat() const;
        [[nodiscard]] inline VkFormat GetDepthImageFormat() const;

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

#include <FlashlightEngine/VulkanRenderer/VulkanRenderer.inl>
}
