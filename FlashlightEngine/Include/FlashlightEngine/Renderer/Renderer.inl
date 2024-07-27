/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Renderer.inl
 * Description : Definitions of inline methods from the Renderer class.
 */
#pragma once

inline void Renderer::EndRenderPass(const VkCommandBuffer commandBuffer) {
    vkCmdEndRenderPass(commandBuffer);
}

inline void Renderer::UseMainPipeline(const VkCommandBuffer commandBuffer) const {
    m_MainGraphicsPipeline->Bind(commandBuffer);
}

inline std::shared_ptr<VulkanWrapper::Instance> Renderer::GetInstance() const {
    return m_Instance;
}

inline std::shared_ptr<VulkanWrapper::Device> Renderer::GetDevice() const {
    return m_Device;
}

inline const FrameObjects& Renderer::GetCurrentFrameObjects() const {
    return m_FrameObjects[m_CurrentFrameNumber % m_MaxFramesInFlight];
}
