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
