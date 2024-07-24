/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : GraphicsPipeline.inl
 * Description : Definitions of inline methods from the GraphicsPipeline class.
 */
#pragma once

inline GraphicsPipeline::Builder::Builder(Device& device) : m_Device(device) {}

inline GraphicsPipeline::~GraphicsPipeline() {
    if (m_Pipeline) {
        Log::EngineTrace("Destroying Vulkan pipeline.");
        vkDestroyPipeline(m_Device.GetNativeDevice(), m_Pipeline, nullptr);
    }
    
    if (m_PipelineLayout) {
        vkDestroyPipelineLayout(m_Device.GetNativeDevice(), m_PipelineLayout, nullptr);
    }
}

inline VkPipeline GraphicsPipeline::GetNativePipeline() const {
    return m_Pipeline;
}

inline VkPipelineLayout GraphicsPipeline::GetNativePipelineLayout() const {
    return m_PipelineLayout;
}

inline void GraphicsPipeline::Bind(const VkCommandBuffer commandBuffer) const {
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline);
}
