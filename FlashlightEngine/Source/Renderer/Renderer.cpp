/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Renderer.cpp
 * Description : Definitions of methods from the Renderer class.
 */
#include "FlashlightEngine/Renderer/Renderer.hpp"

#include "FlashlightEngine/Renderer/RendererStructures/Vertex.hpp"
#include "FlashlightEngine/Renderer/VulkanWrapper/Instance.hpp"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Flashlight {
    Renderer::Renderer(const DebugLevel& debugLevel, Window& window) : m_Window(window) {
        InitializeVulkan(debugLevel, window);
        CreateDescriptorSetLayout();
        CreatePipeline();
        CreateCommandPool();
        AllocateCommandBuffers();
        CreateSynchronisationPrimitives();
        CreateUniformBuffers();
        CreateDescriptorPool();
        CreateDescriptorSets();
    }

    Renderer::~Renderer() {
        vkDeviceWaitIdle(m_Device->GetNativeDevice());

        for (const auto& frame : m_FrameObjects) {
            frame.UniformBuffer->Unmap();

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

        vkDestroyDescriptorPool(m_Device->GetNativeDevice(), m_DescriptorPool, nullptr);

        vkDestroyDescriptorSetLayout(m_Device->GetNativeDevice(), m_DescriptorSetLayout, nullptr);
    }

    VkCommandBuffer Renderer::BeginFrame() {
        m_CurrentFrameNumber++;
        const auto& frame = GetCurrentFrameObjects();
        const auto result = m_SwapChain->AcquireNextImageIndex(frame, m_CurrentFrameIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            RecreateSwapChain();
            m_Window.SwapChainRecreated();
            return nullptr;
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
        auto& frame = GetCurrentFrameObjects();
        
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

        UseMainPipeline(commandBuffer);

        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_MainGraphicsPipeline->GetNativePipelineLayout(), 0, 1, &frame.DescriptorSet, 0, nullptr);
    }

    void Renderer::EndFrame() {
        const auto& frame = GetCurrentFrameObjects();
        if (vkEndCommandBuffer(frame.FrameCommandBuffer) != VK_SUCCESS) {
            Log::EngineError("Failed to record command buffer.");
        }
        const auto result = m_SwapChain->SubmitCommandBufferAndPresent(frame, m_CurrentFrameIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_Window.
            ShouldRecreateSwapChain()) {
            RecreateSwapChain();
            m_Window.SwapChainRecreated();
            return;
        }
        if (result != VK_SUCCESS) {
            Log::EngineError("Failed to present swap chain image.");
        }
    }

    void Renderer::UpdateUniforms(const float deltaTime) const {
        auto& frame = GetCurrentFrameObjects();

        const float aspectRatio = static_cast<float>(m_SwapChain->GetSwapChainExtent().width) /
            static_cast<float>(m_SwapChain->GetSwapChainExtent().height);

        UniformBufferObject ubo;
        ubo.Model = rotate(glm::mat4(1.0f), deltaTime * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.View = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f),
                               glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.Projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 10.0f);

        ubo.Projection[1][1] *= -1;

        frame.UniformBuffer->SendData(sizeof(ubo), &ubo);
    }

    void Renderer::InitializeVulkan(const DebugLevel& debugLevel, const Window& window) {
        m_Instance = std::make_shared<VulkanWrapper::Instance>(debugLevel);

        if (debugLevel > DebugLevel::None) {
            m_DebugMessenger = std::make_unique<VulkanWrapper::DebugMessenger>(m_Instance, debugLevel);
        }

        m_Surface = std::make_unique<VulkanWrapper::Surface>(m_Instance, window);

        m_Device = std::make_shared<VulkanWrapper::Device>(m_Instance, *m_Surface, debugLevel);

        VulkanWrapper::SwapChainDescription description{};
        description.Surface = m_Surface->GetNativeSurface();
        description.WindowExtent = m_Window.GetWindowExtent();
        description.OldSwapChain = nullptr;

        m_SwapChain = std::make_unique<VulkanWrapper::SwapChain>(m_Device, description);
    }

    void Renderer::CreateDescriptorSetLayout() {
        m_UboLayoutBinding = {};
        m_UboLayoutBinding.binding = 0;
        m_UboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        m_UboLayoutBinding.descriptorCount = 1;
        m_UboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        m_UboLayoutBinding.pImmutableSamplers = nullptr;

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = 1;
        layoutInfo.pBindings = &m_UboLayoutBinding;

        if (vkCreateDescriptorSetLayout(m_Device->GetNativeDevice(), &layoutInfo, nullptr, &m_DescriptorSetLayout)
            != VK_SUCCESS) {
            Log::EngineFatal({0x02, 0x11}, "Failed to create descriptor set layout.");
        }
    }

    void Renderer::CreatePipeline() {
        const auto vertexShaderModule = std::make_shared<VulkanWrapper::ShaderModule>(
            m_Device, "Shaders/basic.vert", VulkanWrapper::ShaderType::Vertex);
        const auto fragmentShaderModule = std::make_shared<VulkanWrapper::ShaderModule>(
            m_Device, "Shaders/basic.frag", VulkanWrapper::ShaderType::Fragment);

        const auto bindings = Vertex2DColor::GetBindingDescription();
        const auto attributes = Vertex2DColor::GetAttributesDescriptions();

        VulkanWrapper::GraphicsPipeline::Builder pipelineBuilder{m_Device};
        pipelineBuilder.VertexShader(vertexShaderModule);
        pipelineBuilder.FragmentShader(fragmentShaderModule);
        pipelineBuilder.VertexInputState(bindings, attributes);
        pipelineBuilder.InputAssemblyState(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, false);
        pipelineBuilder.Viewport(m_SwapChain->GetSwapChainExtent());
        pipelineBuilder.RasterizeState(VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT);
        pipelineBuilder.MultisampleState();
        pipelineBuilder.ColorBlendState(false, VK_BLEND_OP_ADD, VK_BLEND_OP_ADD, false, VK_LOGIC_OP_COPY);
        pipelineBuilder.PipelineLayout({m_DescriptorSetLayout}, {});

        m_MainGraphicsPipeline = pipelineBuilder.Build(m_SwapChain->GetRenderPass());
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
                Log::EngineFatal({0x02, 0x10}, "Failed to create synchronisation primitives.");
            }
        }
    }

    void Renderer::CreateUniformBuffers() {
        for (auto& frame : m_FrameObjects) {
            constexpr VkDeviceSize bufferSize = sizeof(UniformBufferObject);
            frame.UniformBuffer = std::make_unique<VulkanWrapper::Buffer>(
                m_Device, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

            frame.UniformBuffer->Map(0, bufferSize);
        }
    }

    void Renderer::CreateDescriptorPool() {
        VkDescriptorPoolSize poolSize;
        poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSize.descriptorCount = m_MaxFramesInFlight;

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = 1;
        poolInfo.pPoolSizes = &poolSize;
        poolInfo.maxSets = m_MaxFramesInFlight;

        if (vkCreateDescriptorPool(m_Device->GetNativeDevice(), &poolInfo, nullptr, &m_DescriptorPool) != VK_SUCCESS) {
            Log::EngineFatal({0x02, 0x12}, "Failed to create descriptor pool.");
        }
    }

    void Renderer::CreateDescriptorSets() {

        for (auto& frame : m_FrameObjects) {
            VkDescriptorSetAllocateInfo allocateInfo{};
            allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            allocateInfo.descriptorPool = m_DescriptorPool;
            allocateInfo.descriptorSetCount = 1;
            allocateInfo.pSetLayouts = &m_DescriptorSetLayout;
            
            if (vkAllocateDescriptorSets(m_Device->GetNativeDevice(), &allocateInfo, &frame.DescriptorSet) != VK_SUCCESS) {
                Log::EngineFatal({0x02, 0x13}, "Failed to create frame descriptor set.");
            }

            VkDescriptorBufferInfo bufferInfo = frame.UniformBuffer->GetDescriptorInfo(sizeof(UniformBufferObject));

            VkWriteDescriptorSet descriptorWrite{};
            descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrite.dstSet = frame.DescriptorSet;
            descriptorWrite.dstBinding = 0;
            descriptorWrite.dstArrayElement = 0;
            descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrite.descriptorCount = 1;
            descriptorWrite.pBufferInfo = &bufferInfo;
            descriptorWrite.pImageInfo = nullptr;
            descriptorWrite.pTexelBufferView = nullptr;
            
            vkUpdateDescriptorSets(m_Device->GetNativeDevice(), 1, &descriptorWrite, 0, nullptr);
        }
    }

    void Renderer::RecreateSwapChain() {
        auto extent = m_Window.GetWindowExtent();
        while (extent.width == 0 || extent.height == 0) {
            extent = m_Window.GetWindowExtent();
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(m_Device->GetNativeDevice());

        VulkanWrapper::SwapChainDescription description{};
        description.Surface = m_Surface->GetNativeSurface();
        description.WindowExtent = extent;
        description.OldSwapChain = std::move(m_SwapChain);

        m_SwapChain = std::make_unique<VulkanWrapper::SwapChain>(m_Device, description);
    }
}
