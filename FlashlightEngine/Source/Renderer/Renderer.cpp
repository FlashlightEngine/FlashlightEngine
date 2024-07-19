/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Renderer.cpp
 * Description : Definitions of methods from the Renderer class.
 */
#include "FlashlightEngine/Renderer/Renderer.hpp"

namespace Flashlight {
    Renderer::Renderer(const Window &window) {
        m_Instance = std::make_unique<WGPUWrapper::Instance>();
        m_Device = std::make_unique<WGPUWrapper::Device>(*m_Instance, window);
        m_Queue = std::make_unique<WGPUWrapper::Queue>(*m_Device);
    }


    void Renderer::Initialize() {
    }

    WGPUWrapper::CommandEncoder Renderer::BeginRecordCommands() const {
        WGPUWrapper::CommandEncoder commandEncoder{*m_Device};

        return commandEncoder;
    }

    WGPUCommandBuffer Renderer::EndRecordCommands(const WGPUWrapper::CommandEncoder& commandEncoder) {
        WGPUCommandBufferDescriptor commandBufferDescriptor = {};
        commandBufferDescriptor.nextInChain = nullptr;
        commandBufferDescriptor.label = "Command buffer";

        WGPUCommandBuffer commandBuffer = wgpuCommandEncoderFinish(
            commandEncoder.GetNativeEncoder(), &commandBufferDescriptor);

        return commandBuffer;
    }
}
