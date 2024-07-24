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
    Renderer::Renderer(const DebugLevel& debugLevel, const Window& window) {
        InitializeVulkan(debugLevel, window);
        CreatePipeline();
        CreateCommandPool();
    }

    Renderer::~Renderer() {
        Log::EngineTrace("Destroying Vulkan command pool.");
        if (m_CommandPool) {
            vkDestroyCommandPool(m_Device->GetNativeDevice(), m_CommandPool, nullptr);
        }
    }

    void Renderer::InitializeVulkan(const DebugLevel& debugLevel, const Window& window) {
        m_Instance = std::make_unique<VulkanWrapper::Instance>(debugLevel);

        if (debugLevel > DebugLevel::None) {
            m_DebugMessenger = std::make_unique<VulkanWrapper::DebugMessenger>(*m_Instance, debugLevel);
        }

        m_Surface = std::make_unique<VulkanWrapper::Surface>(*m_Instance, window);

        m_Device = std::make_unique<VulkanWrapper::Device>(*m_Instance, *m_Surface, debugLevel);

        m_SwapChain = std::make_unique<VulkanWrapper::SwapChain>(*m_Device, window, *m_Surface);
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
}
