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

namespace Flashlight::Renderer {

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
        Bounds Bounds;
        glm::mat4 Transform;
        VkDeviceAddress VertexBufferAddress;
    };

    struct DrawContext {
        std::vector<RenderObject> OpaqueSurfaces;
        std::vector<RenderObject> TransparentSurfaces;
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

        DescriptorAllocatorGrowable m_GlobalDescriptorAllocator;

        VkDescriptorSet m_DrawImageDescriptors;
        VkDescriptorSetLayout m_DrawImageDescriptorLayout;

        VkDescriptorSetLayout m_SingleImageDescriptorLayout;

        MaterialInstance m_DefaultData;

    public:
        AllocatedImage DrawImage;
        AllocatedImage DepthImage;
        VkExtent2D DrawExtent;
        
        VkDescriptorSetLayout GpuSceneDataLayout;

        std::unordered_map<std::string, std::shared_ptr<LoadedGLTF>> LoadedScenes;

        AllocatedImage WhiteImage;
        AllocatedImage BlackImage;
        AllocatedImage GrayImage;
        AllocatedImage ErrorCheckerboardImage;

        VkSampler DefaultSamplerLinear;
        VkSampler DefaultSamplerNearest;
        
        GLTFMetallic_Roughness MetalRoughMaterial;
        
        VulkanRenderer(const Window& window, const DebugLevel& debugLevel);
        ~VulkanRenderer();

        VulkanRenderer(const VulkanRenderer&) = delete;
        VulkanRenderer(VulkanRenderer&&) = delete;

        VulkanRenderer& operator=(const VulkanRenderer&) = delete;
        VulkanRenderer& operator=(VulkanRenderer&&) = delete;

        [[nodiscard]] GPUMeshBuffers UploadMesh(std::span<u32> indices, std::span<Vertex> vertices) const;
        void PlanMeshDeletion(GPUMeshBuffers mesh);
        void PlanDescriptorPoolsDeletion(DescriptorAllocatorGrowable& allocator);
        void PlanDeletion(std::function<void()>&& deletor);
        static void BeginUi();
        void BeginRendering(const Window& window, VkClearColorValue clearColor, EngineStats& stats);
        void EndRendering(Window& window);

        void ImmediateSubmit(const std::function<void(VkCommandBuffer commandBuffer)>& function) const;

        [[nodiscard]] inline VulkanWrapper::Instance& GetInstance() const;
        [[nodiscard]] inline VulkanWrapper::Device& GetDevice() const;
        [[nodiscard]] inline VmaAllocator GetAllocator() const;
        [[nodiscard]] inline VkFormat GetDrawImageFormat() const;
        [[nodiscard]] inline VkFormat GetDepthImageFormat() const;

        [[nodiscard]] FrameData& GetCurrentFrame() {
            return m_Frames[m_FrameNumber % g_FrameOverlap];
        }

        [[nodiscard]] static bool IsVisible(const RenderObject& object, const glm::mat4& viewProj);

    private:
        void InitializeVulkan(const Window& window, const DebugLevel& debugLevel);
        void InitializeSwapchain(const Window& window);
        void InitializeCommands();
        void InitializeFramesCommandBuffers();
        void InitializeImmediateCommandBuffer();
        void InitializeSynchronisationPrimitives();
        void InitializeDescriptors();
        void InitializePipelines();
        void InitializeImGui(const Window& window);
        void InitializeDefaultData();

        void DrawImGui(VkCommandBuffer commandBuffer, VkImageView targetImageView) const;

        void RecreateSwapchain(Window& window);
    };

#include <FlashlightEngine/VulkanRenderer/VulkanRenderer.inl>
}
