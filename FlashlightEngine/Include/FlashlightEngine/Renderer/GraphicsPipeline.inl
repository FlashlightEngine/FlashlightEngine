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
                                          const std::filesystem::path& fragmentShaderPath) : m_Device(
    device.GetNativeDevice()) {
    Create(vertexShaderPath, fragmentShaderPath);
}

inline GraphicsPipeline::~GraphicsPipeline() {
    Destroy();
}

inline VkPipeline GraphicsPipeline::GetNativePipeline() const {
    return m_GraphicsPipeline;
}

inline void GraphicsPipeline::Destroy() const {
    if (m_GraphicsPipeline) {
        vkDestroyPipeline(m_Device, m_GraphicsPipeline, nullptr);
    }
}
