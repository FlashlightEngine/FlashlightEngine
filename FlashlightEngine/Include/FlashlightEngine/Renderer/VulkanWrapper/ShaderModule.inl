/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : ShaderModule.inl
 * Description : Definitions of inline methods from the ShaderModule class.
 */
#pragma once

inline ShaderModule::~ShaderModule() {
    if (m_ShaderModule) {
        Log::EngineTrace("Destroying Vulkan shader module.");
        vkDestroyShaderModule(m_Device->GetNativeDevice(), m_ShaderModule, nullptr);
    }
}

inline VkShaderModule ShaderModule::GetNativeShaderModule() const {
    return m_ShaderModule;
}

inline VkPipelineShaderStageCreateInfo ShaderModule::GetNativeShaderStageInfo() const {
    return m_ShaderStageInfo;
}