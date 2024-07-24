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
        m_Instance = std::make_unique<VulkanWrapper::Instance>(debugLevel);

        if (debugLevel > DebugLevel::None) {
            m_DebugMessenger = std::make_unique<VulkanWrapper::DebugMessenger>(*m_Instance, debugLevel);
        }

        m_Surface = std::make_unique<VulkanWrapper::Surface>(*m_Instance, window);

        m_Device = std::make_unique<VulkanWrapper::Device>(*m_Instance, *m_Surface, debugLevel);

        m_SwapChain = std::make_unique<VulkanWrapper::SwapChain>(*m_Device, window, *m_Surface);

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
}
