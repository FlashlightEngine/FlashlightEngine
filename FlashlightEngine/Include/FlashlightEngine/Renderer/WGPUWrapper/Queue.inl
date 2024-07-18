/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Queue.inl
 * Description : Definitions of inline methods from the Queue class.
 */

#pragma once

inline Queue::Queue(const Device& device) {
    Create(device.GetNativeDevice());
}

inline Queue::~Queue() {
    Destroy();
}

inline void Queue::Submit(const size commandNumber, const std::vector<WGPUCommandBuffer>& commandBuffers) const {
    wgpuQueueSubmit(m_Queue, commandNumber, commandBuffers.data());
}

inline void Queue::WriteBuffer(WGPUBuffer buffer, const u64 offset, void const* data, const size bufferSize) const {
    wgpuQueueWriteBuffer(m_Queue, buffer, offset, data, bufferSize);
}

inline void Queue::WriteTexture(WGPUImageCopyTexture const* destination, void const* data, size dataSize,
                                WGPUTextureDataLayout const* dataLayout, WGPUExtent3D const* writeSize) const {
    wgpuQueueWriteTexture(m_Queue, destination, data, dataSize, dataLayout, writeSize);
}
