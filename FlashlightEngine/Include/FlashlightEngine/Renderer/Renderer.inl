/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Renderer.inl
 * Description : Definitions of inline methods from the Renderer class.
 */
#pragma once

inline WGPUWrapper::Device Renderer::GetDevice() const {
    return *m_Device;
}

inline WGPUWrapper::Queue Renderer::GetQueue() const {
    return *m_Queue;
}

inline void Renderer::SubmitCommandBuffers(const std::vector<WGPUCommandBuffer>& commandBuffers) const {
    m_Queue->Submit(commandBuffers);

    for (int i = 0; i < 5; ++i) {
        wgpuDevicePoll(m_Device->GetNativeDevice(), false, nullptr);
    }
}