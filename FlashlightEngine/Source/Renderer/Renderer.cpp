/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Renderer.cpp
 * Description : Definitions of methods from the Renderer class.
 */
#include "FlashlightEngine/Renderer/Renderer.hpp"

#include "FlashlightEngine/Renderer/VulkanWrapper/Instance.hpp"

namespace Flashlight {
    Renderer::Renderer(const DebugLevel& debugLevel, Window& window) : m_Window(window) {
        InitializeVulkan(debugLevel, window);
        RecreateSwapChain();
        CreatePipeline();
        CreateCommandPool();
        AllocateCommandBuffers();
        CreateSynchronisationPrimitives();
    }

    Renderer::~Renderer() {
        vkDeviceWaitIdle(m_Device->GetNativeDevice());

        for (const auto frame : m_FrameObjects) {
            if (frame.ImageAvailableSemaphore && frame.RenderFinishedSemaphore && frame.InFlightFence) {
                vkDestroySemaphore(m_Device->GetNativeDevice(), frame.ImageAvailableSemaphore, nullptr);
                vkDestroySemaphore(m_Device->GetNativeDevice(), frame.RenderFinishedSemaphore, nullptr);
                vkDestroyFence(m_Device->GetNativeDevice(), frame.InFlightFence, nullptr);
            }
        }

        if (m_CommandPool) {
            Log::EngineTrace("Destroying Vulkan command pool.");
            vkDestroyCommandPool(m_Device->GetNativeDevice(), m_CommandPool, nullptr);
        }
    }

    VkCommandBuffer Renderer::BeginFrame() {
        m_CurrentFrameNumber++;
        const auto& frame = GetCurrentFrameObjects();
        const auto result = m_SwapChain->AcquireNextImageIndex(frame, m_CurrentFrameIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            RecreateSwapChain();
            m_Window.SwapChainInvalidated();
        }
        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            Log::EngineError("Failed to present swap chain image.");
        }

        vkResetCommandBuffer(frame.FrameCommandBuffer, 0);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0;
        beginInfo.pInheritanceInfo = nullptr;

        if (vkBeginCommandBuffer(frame.FrameCommandBuffer, &beginInfo) != VK_SUCCESS) {
            Log::EngineError("Failed to begin command buffer recording.");
        }

        return frame.FrameCommandBuffer;
    }

    void Renderer::BeginRenderPass(const VkCommandBuffer commandBuffer) const {
        VkRenderPassBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        beginInfo.renderPass = m_SwapChain->GetRenderPass().GetNativeRenderPass();
        beginInfo.framebuffer = m_SwapChain->GetFramebufferAtIndex(m_CurrentFrameIndex);
        beginInfo.renderArea.offset = {0, 0};
        beginInfo.renderArea.extent = m_SwapChain->GetSwapChainExtent();

        constexpr VkClearValue clearColor = {{{0.01f, 0.01f, 0.01f, 1.0f}}};
        beginInfo.clearValueCount = 1;
        beginInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass(commandBuffer, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport;
        viewport.x = 0;
        viewport.y = 0;
        viewport.width = static_cast<float>(m_SwapChain->GetSwapChainExtent().width);
        viewport.height = static_cast<float>(m_SwapChain->GetSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

        VkRect2D scissor;
        scissor.offset = {0, 0};
        scissor.extent = m_SwapChain->GetSwapChainExtent();
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
    }

    void Renderer::EndFrame() {
        const auto& frame = GetCurrentFrameObjects();
        if (vkEndCommandBuffer(frame.FrameCommandBuffer) != VK_SUCCESS) {
            Log::EngineError("Failed to record command buffer.");
        }
        const auto result = m_SwapChain->SubmitCommandBufferAndPresent(frame, m_CurrentFrameIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_Window.
            ShouldInvalidateSwapChain()) {
            RecreateSwapChain();
            m_Window.SwapChainInvalidated();
        }
        if (result != VK_SUCCESS) {
            Log::EngineError("Failed to present swap chain image.");
        }
    }

    void Renderer::InitializeVulkan(const DebugLevel& debugLevel, const Window& window) {
        m_Instance = std::make_unique<VulkanWrapper::Instance>(debugLevel);

        if (debugLevel > DebugLevel::None) {
            m_DebugMessenger = std::make_unique<VulkanWrapper::DebugMessenger>(*m_Instance, debugLevel);
        }

        m_Surface = std::make_unique<VulkanWrapper::Surface>(*m_Instance, window);

        m_Device = std::make_unique<VulkanWrapper::Device>(*m_Instance, *m_Surface, debugLevel);
    }

    void Renderer::CreatePipeline() {
        VulkanWrapper::ShaderModule vertexShaderModule{
            *m_Device, "Shaders/basic.vert", VulkanWrapper::ShaderType::Vertex
        };
        VulkanWrapper::ShaderModule fragmentShaderModule{
            *m_Device, "Shaders/basic.frag", VulkanWrapper::ShaderType::Fragment
        };

        VulkanWrapper::GraphicsPipeline::Builder pipelineBuilder{*m_Device};
        pipelineBuilder.VertexShader(vertexShaderModule);
        pipelineBuilder.FragmentShader(fragmentShaderModule);
        pipelineBuilder.VertexInputState({}, {});
        pipelineBuilder.InputAssemblyState(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, false);
        pipelineBuilder.Viewport(m_SwapChain->GetSwapChainExtent());
        pipelineBuilder.RasterizeState(VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT);
        pipelineBuilder.MultisampleState();
        pipelineBuilder.ColorBlendState(false, VK_BLEND_OP_ADD, VK_BLEND_OP_ADD, false, VK_LOGIC_OP_COPY);
        pipelineBuilder.PipelineLayout({}, {});

        m_GraphicsPipeline = pipelineBuilder.Build(m_SwapChain->GetRenderPass());
    }

    void Renderer::CreateCommandPool() {
        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = m_Device->GetQueueFamilies().GraphicsFamily;

        Log::EngineTrace("Creating Vulkan command pool.");
        if (vkCreateCommandPool(m_Device->GetNativeDevice(), &poolInfo, nullptr, &m_CommandPool) != VK_SUCCESS) {
            Log::EngineFatal({0x01, 0x0E}, "Failed to create command pool.");
        }
        Log::EngineTrace("Vulkan command pool created");
    }

    void Renderer::AllocateCommandBuffers() {
        VkCommandBufferAllocateInfo allocateInfo{};
        allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocateInfo.commandPool = m_CommandPool;
        allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocateInfo.commandBufferCount = 1;

        for (auto& frame : m_FrameObjects) {
            if (vkAllocateCommandBuffers(m_Device->GetNativeDevice(), &allocateInfo,
                                         &frame.FrameCommandBuffer)
                != VK_SUCCESS) {
                Log::EngineFatal({0x01, 0x0F}, "Failed to allocate command buffer.");
            }
        }
    }

    void Renderer::CreateSynchronisationPrimitives() {
        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (auto& frame : m_FrameObjects) {
            if (vkCreateSemaphore(m_Device->GetNativeDevice(), &semaphoreInfo, nullptr,
                                  &frame.ImageAvailableSemaphore) ||
                vkCreateSemaphore(m_Device->GetNativeDevice(), &semaphoreInfo, nullptr,
                                  &frame.RenderFinishedSemaphore) ||
                vkCreateFence(m_Device->GetNativeDevice(), &fenceInfo, nullptr, &frame.InFlightFence)
                != VK_SUCCESS) {
                Log::EngineFatal({0x01, 0x10}, "Failed to create synchronisation primitives.");
            }
        }
    }

    void Renderer::RecreateSwapChain() {
        i32 width = 0, height = 0;
        glfwGetFramebufferSize(m_Window.GetGlfwWindow(), &width, &height);
        while (width == 0 || height == 0) {
            glfwGetFramebufferSize(m_Window.GetGlfwWindow(), &width, &height);
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(m_Device->GetNativeDevice());

        VulkanWrapper::SwapChainDescription description{};
        description.Surface = m_Surface->GetNativeSurface();
        description.WindowExtent = {static_cast<u32>(width), static_cast<u32>(height)};
        description.OldSwapChain = std::move(m_SwapChain);

        m_SwapChain = std::make_unique<VulkanWrapper::SwapChain>(*m_Device, description);
    }
}
