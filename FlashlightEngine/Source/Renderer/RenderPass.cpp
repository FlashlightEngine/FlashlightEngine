/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : RenderPass.cpp
 * Description : Definitions of methods from the RenderPass class.
 */
#include "FlashlightEngine/Renderer/RenderPass.hpp"

#include "FlashlightEngine/pch.hpp"

namespace Flashlight {
    void RenderPass::Begin(const CommandEncoder& commandEncoder, WGPUTextureView textureView,
                           const WGPUColor& clearColor) {
        WGPURenderPassDescriptor renderPassDescriptor = {};
        renderPassDescriptor.nextInChain = nullptr;

        WGPURenderPassColorAttachment renderPassColorAttachment = {};
        renderPassColorAttachment.view = textureView;
        renderPassColorAttachment.resolveTarget = nullptr;
        renderPassColorAttachment.loadOp = WGPULoadOp_Clear;
        renderPassColorAttachment.storeOp = WGPUStoreOp_Store;
        renderPassColorAttachment.clearValue = clearColor;

        renderPassDescriptor.colorAttachmentCount = 1;
        renderPassDescriptor.colorAttachments = &renderPassColorAttachment;
        renderPassDescriptor.depthStencilAttachment = nullptr;
        renderPassDescriptor.timestampWrites = nullptr;

        m_RenderPass = wgpuCommandEncoderBeginRenderPass(commandEncoder.GetNativeEncoder(), &renderPassDescriptor);
    }
}
