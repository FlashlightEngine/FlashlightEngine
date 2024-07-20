/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : RenderPass.inl
 * Description : Definitions of inline methods from the RenderPass class.
 */
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