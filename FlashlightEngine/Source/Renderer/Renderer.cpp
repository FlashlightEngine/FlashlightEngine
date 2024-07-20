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

    WGPUTextureView Renderer::GetNextSwapChainTextureView() const {
        WGPUSurfaceTexture surfaceTexture;
        wgpuSurfaceGetCurrentTexture(m_Device->GetWindowSurface(), &surfaceTexture);

        if (surfaceTexture.status != WGPUSurfaceGetCurrentTextureStatus_Success) {
            return nullptr;
        }

        WGPUTextureViewDescriptor viewDescriptor;
        viewDescriptor.nextInChain = nullptr;
        viewDescriptor.label = "Surface texture view";
        viewDescriptor.format = wgpuTextureGetFormat(surfaceTexture.texture);
        viewDescriptor.dimension = WGPUTextureViewDimension_2D;
        viewDescriptor.baseMipLevel = 0;
        viewDescriptor.mipLevelCount = 1;
        viewDescriptor.baseArrayLayer = 0;
        viewDescriptor.arrayLayerCount = 1;
        viewDescriptor.aspect = WGPUTextureAspect_All;
        WGPUTextureView targetView = wgpuTextureCreateView(surfaceTexture.texture, &viewDescriptor);

        return targetView;
    }

    CommandEncoder Renderer::BeginRecordCommands() const {
        CommandEncoder commandEncoder{*m_Device};

        return commandEncoder;
    }

    WGPUCommandBuffer Renderer::EndRecordCommands(const CommandEncoder& commandEncoder) {
        WGPUCommandBufferDescriptor commandBufferDescriptor = {};
        commandBufferDescriptor.nextInChain = nullptr;
        commandBufferDescriptor.label = "Command buffer";

        WGPUCommandBuffer commandBuffer = wgpuCommandEncoderFinish(
            commandEncoder.GetNativeEncoder(), &commandBufferDescriptor);

        return commandBuffer;
    }
}
