/* Copyright (C) 2024 Jean "Pixfri" Letessier
* This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : RenderPass.hpp
 * Description : Declaration of the RenderPass class. A render pass is a specialised command encoder object for drawing
 *               commands.
 */
#pragma once

#include "FlashlightEngine/Renderer/CommandEncoder.hpp"

#include "FlashlightEngine/pch.hpp"

namespace Flashlight {
    class RenderPass {
        WGPURenderPassEncoder m_RenderPass;

    public:
        RenderPass() = default;
        inline ~RenderPass();

        RenderPass(const RenderPass&) = default;
        RenderPass(RenderPass&&) = default;

        RenderPass& operator=(const RenderPass&) = default;
        RenderPass& operator=(RenderPass&&) = default;

        void Begin(const CommandEncoder& commandEncoder, WGPUTextureView textureView,
                   const WGPUColor& clearColor);
        inline void End() const;

    private:
        inline void Destroy() const;
    };

#include "RenderPass.inl"
}
