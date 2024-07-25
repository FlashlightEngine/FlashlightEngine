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

#include "FlashlightEngine/Renderer/RendererStructures.hpp"
#include "FlashlightEngine/Renderer/VulkanWrapper/Instance.hpp"
#include "FlashlightEngine/Renderer/VulkanWrapper/DebugMessenger.hpp"
#include "FlashlightEngine/Renderer/VulkanWrapper/Surface.hpp"
#include "FlashlightEngine/Renderer/VulkanWrapper/Device.hpp"
#include "FlashlightEngine/Renderer/VulkanWrapper/SwapChain.hpp"
#include "FlashlightEngine/Renderer/VulkanWrapper/DescriptorSet.hpp"
#include "FlashlightEngine/Renderer/VulkanWrapper/GraphicsPipeline.hpp"

namespace Flashlight {

    class Renderer {
        u8 m_MaxFramesInFlight = 2;

        std::unique_ptr<VulkanWrapper::Instance> m_Instance;
        std::unique_ptr<VulkanWrapper::DebugMessenger> m_DebugMessenger;
        std::unique_ptr<VulkanWrapper::Surface> m_Surface;
        std::unique_ptr<VulkanWrapper::Device> m_Device;
        std::unique_ptr<VulkanWrapper::SwapChain> m_SwapChain;
        std::unique_ptr<VulkanWrapper::DescriptorSetLayout> m_DescriptorSetLayout;
        std::unique_ptr<VulkanWrapper::GraphicsPipeline> m_GraphicsPipeline;

        VkCommandPool m_CommandPool = VK_NULL_HANDLE;

        std::vector<Frame> m_Frames {m_MaxFramesInFlight};
        
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

        [[nodiscard]] inline VulkanWrapper::Instance& GetInstance() const;
        [[nodiscard]] inline VulkanWrapper::Surface& GetSurface() const;
        [[nodiscard]] inline VulkanWrapper::Device& GetDevice() const;
        [[nodiscard]] inline VulkanWrapper::SwapChain& GetSwapChain() const;
        [[nodiscard]] inline VulkanWrapper::GraphicsPipeline& GetPipeline() const;

    private:
        void InitializeVulkan(const DebugLevel& debugLevel, const Window& window);
        void CreatePipeline();
        void CreateCommandPool();
        void CreateFrameResources();

        void AllocateFramesCommandBuffer();
        void CreateFramesSynchronisationPrimitives();
        void AllocateFramesUniformBuffers();

        void FreeFrameResources() const;

        void RecreateSwapChain();

        [[nodiscard]] inline const Frame& GetCurrentFrame() const;
        [[nodiscard]] inline u32 GetCurrentFrameIndex() const;
        void UpdateUniformBuffer() const;
    };

#include "Renderer.inl"
}
