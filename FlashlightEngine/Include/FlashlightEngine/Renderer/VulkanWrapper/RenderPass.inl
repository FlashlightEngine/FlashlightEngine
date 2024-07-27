/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : RenderPass.inl
 * Description : Definitions of inline methods from the RenderPass class.
 */
#pragma once

inline RenderPass::~RenderPass() {
    if (m_RenderPass) {
        Log::EngineTrace("Destroying Vulkan render pass.");
        vkDestroyRenderPass(m_Device->GetNativeDevice(), m_RenderPass, nullptr);
    }
}

inline VkRenderPass RenderPass::GetNativeRenderPass() const {
    return m_RenderPass;
}
