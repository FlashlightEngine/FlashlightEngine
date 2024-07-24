/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : GraphicsPipeline.inl
 * Description : Definitions of inline methods from the GraphicsPipeline class.
 */
#pragma once

inline GraphicsPipeline::GraphicsPipeline(const VulkanWrapper::Device& device,
                                          const std::filesystem::path& vertexShaderPath,
                                          const std::filesystem::path& fragmentShaderPath,
                                          const PipelineInfos& pipelineInfos,
                                          const RenderPass& renderPass)
    : m_Device(device.GetNativeDevice()), m_RenderPass(renderPass.GetNativeRenderPass()) {
    Create(vertexShaderPath, fragmentShaderPath, pipelineInfos);
}

inline GraphicsPipeline::~GraphicsPipeline() {
    Destroy();
}

inline VkPipeline GraphicsPipeline::GetNativePipeline() const {
    return m_Pipeline;
}

inline void GraphicsPipeline::Destroy() const {
    if (m_Pipeline) {
        vkDestroyPipeline(m_Device, m_Pipeline, nullptr);
    }
    
    if (m_PipelineLayout) {
        vkDestroyPipelineLayout(m_Device, m_PipelineLayout, nullptr);
    }
}
