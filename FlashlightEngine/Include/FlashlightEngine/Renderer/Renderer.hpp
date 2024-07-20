/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Renderer.hpp
 * Description : Declaration of the Renderer class. This class manages objects required for rendering, such as
 *               the WebGPU instance and device.
 */
#pragma once

#include "FlashlightEngine/Renderer/WGPUWrapper/Instance.hpp"
#include "FlashlightEngine/Renderer/WGPUWrapper/Device.hpp"
#include "FlashlightEngine/Renderer/WGPUWrapper/Queue.hpp"
#include "CommandEncoder.hpp"

#include "FlashlightEngine/Core/Logger.hpp"

#include "FlashlightEngine/pch.hpp"

namespace Flashlight {
    class Renderer {
        std::unique_ptr<WGPUWrapper::Instance> m_Instance;
        std::unique_ptr<WGPUWrapper::Device> m_Device;
        std::unique_ptr<WGPUWrapper::Queue> m_Queue;

    public:
        explicit Renderer(const Window &window);
        ~Renderer() = default;

        static void Initialize();

        [[nodiscard]] inline WGPUWrapper::Device GetDevice() const;
        [[nodiscard]] inline WGPUWrapper::Queue GetQueue() const;
        [[nodiscard]] WGPUTextureView GetNextSwapChainTextureView() const;

        [[nodiscard]] CommandEncoder BeginRecordCommands() const;
        [[nodiscard]] static WGPUCommandBuffer EndRecordCommands(const CommandEncoder &commandEncoder);
        inline void SubmitCommandBuffers(const std::vector<WGPUCommandBuffer> &commandBuffers) const;
        static inline void ReleaseTextureView(WGPUTextureView textureView);

        inline void PresentSurface() const;
    };

#include "Renderer.inl"
}
