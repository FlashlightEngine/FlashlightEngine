/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Renderer.hpp
 * Description : Declaration of the Renderer class. It is used to manage rendering objects. Each application has its
 *               own renderer instance.
 */
#pragma once

#include "FlashlightEngine/Core/Window.hpp"

#include "FlashlightEngine/Renderer/VulkanWrapper/Buffer.hpp"
#include "FlashlightEngine/Renderer/VulkanWrapper/Instance.hpp"
#include "FlashlightEngine/Renderer/VulkanWrapper/DebugMessenger.hpp"
#include "FlashlightEngine/Renderer/VulkanWrapper/Surface.hpp"
#include "FlashlightEngine/Renderer/VulkanWrapper/Device.hpp"
#include "FlashlightEngine/Renderer/VulkanWrapper/SwapChain.hpp"
#include "FlashlightEngine/Renderer/VulkanWrapper/DescriptorSets.hpp"
#include "FlashlightEngine/Renderer/VulkanWrapper/GraphicsPipeline.hpp"

namespace Flashlight {
    struct FrameObjects {
        VkCommandBuffer FrameCommandBuffer;

        VkSemaphore ImageAvailableSemaphore{};
        VkSemaphore RenderFinishedSemaphore{};
        VkFence InFlightFence{};

        std::unique_ptr<VulkanWrapper::Buffer> UniformBuffer;
        VkDescriptorSet DescriptorSet;
    };

    struct UniformBufferObject {
        glm::mat4 Model;
        glm::mat4 View;
        glm::mat4 Projection;
    };

    class Renderer {
        u8 m_MaxFramesInFlight = 2;

        std::shared_ptr<VulkanWrapper::Instance> m_Instance;
        std::unique_ptr<VulkanWrapper::DebugMessenger> m_DebugMessenger;
        std::unique_ptr<VulkanWrapper::Surface> m_Surface;
        std::shared_ptr<VulkanWrapper::Device> m_Device;
        std::unique_ptr<VulkanWrapper::SwapChain> m_SwapChain;
        std::unique_ptr<VulkanWrapper::DescriptorSetLayout> m_DescriptorSetLayout;
        std::unique_ptr<VulkanWrapper::GraphicsPipeline> m_MainGraphicsPipeline;
        std::unique_ptr<VulkanWrapper::DescriptorPool> m_DescriptorPool;
        
        VkCommandPool m_CommandPool = VK_NULL_HANDLE;

        std::vector<FrameObjects> m_FrameObjects{m_MaxFramesInFlight};
        
        u32 m_CurrentFrameIndex = 0;
        u32 m_CurrentFrameNumber = 0;

        Window& m_Window;

    public:
        Renderer(const DebugLevel& debugLevel, Window& window);
        ~Renderer();

        Renderer(const Renderer&) = delete;
        Renderer(Renderer&&) = delete;

        Renderer& operator=(const Renderer&) = delete;
        Renderer& operator=(Renderer&&) = delete;

        [[nodiscard]] VkCommandBuffer BeginFrame();
        void BeginRenderPass(VkCommandBuffer commandBuffer) const;
        static inline void EndRenderPass(VkCommandBuffer commandBuffer);
        void EndFrame();

        inline void UseMainPipeline(VkCommandBuffer commandBuffer) const;
        void UpdateUniforms(float deltaTime) const;

        [[nodiscard]] inline std::shared_ptr<VulkanWrapper::Instance> GetInstance() const;
        [[nodiscard]] inline std::shared_ptr<VulkanWrapper::Device> GetDevice() const;

    private:
        void InitializeVulkan(const DebugLevel& debugLevel, const Window& window);
        void CreateDescriptorSetLayout();
        void CreatePipeline();
        void CreateCommandPool();
        void AllocateCommandBuffers();
        void CreateSynchronisationPrimitives();
        void CreateUniformBuffers();
        void CreateDescriptorPool();
        void CreateDescriptorSets();

        void RecreateSwapChain();

        [[nodiscard]] inline const FrameObjects& GetCurrentFrameObjects() const;
    };

#include "Renderer.inl"
}
