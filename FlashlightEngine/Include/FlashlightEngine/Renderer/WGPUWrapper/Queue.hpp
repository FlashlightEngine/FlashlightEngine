/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Queue.hpp
 * Description : Declaration of the Queue class. The WebGPU queue is used to send commands to the GPU in a batch.
 */
#pragma once

#include "FlashlightEngine/Renderer/WGPUWrapper/Device.hpp"

#include "FlashlightEngine/pch.hpp"

namespace Flashlight::WGPUWrapper {
    class Queue {
        WGPUQueue m_Queue;

    public:
        explicit inline Queue(const Device& device);
        inline ~Queue();

        inline void Submit(const std::vector<WGPUCommandBuffer> &commandBuffers) const;
        inline void WriteBuffer(WGPUBuffer buffer, u64 offset, void const* data, size bufferSize) const;
        inline void WriteTexture(WGPUImageCopyTexture const* destination, void const* data, size dataSize,
                                 WGPUTextureDataLayout const* dataLayout, WGPUExtent3D const* writeSize) const;

    private:
        void Create(WGPUDevice device);
        void Destroy() const;
    };

#include "Queue.inl"
}
