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

inline VulkanWrapper::Instance& Renderer::GetInstance() const {
    return *m_Instance;
}

inline VulkanWrapper::Surface& Renderer::GetSurface() const {
    return *m_Surface;
}

inline VulkanWrapper::Device& Renderer::GetDevice() const {
    return *m_Device;
}

inline VulkanWrapper::SwapChain& Renderer::GetSwapChain() const {
    return *m_SwapChain;
}

inline VulkanWrapper::GraphicsPipeline& Renderer::GetPipeline() const {
    return *m_GraphicsPipeline;
}

inline const Frame& Renderer::GetCurrentFrame() const {
    return m_Frames[m_CurrentFrameNumber % m_MaxFramesInFlight];
}

inline u32 Renderer::GetCurrentFrameIndex() const {
    return m_CurrentFrameNumber % m_MaxFramesInFlight;
}