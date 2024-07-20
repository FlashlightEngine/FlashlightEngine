#pragma once

inline RenderPass::~RenderPass() {
    Destroy();
}

inline void RenderPass::End() const {
    wgpuRenderPassEncoderEnd(m_RenderPass);
}

inline void RenderPass::Destroy() const {
    if (m_RenderPass) {
        wgpuRenderPassEncoderRelease(m_RenderPass);
    }
}