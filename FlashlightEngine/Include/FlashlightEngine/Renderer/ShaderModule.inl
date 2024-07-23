/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : ShaderModule.inl
 * Description : Definitions of inline methods from the ShaderModule class.
 */
#pragma once

inline ShaderModule::ShaderModule(const VulkanWrapper::Device& device, const std::filesystem::path& shaderPath,
                                  const ShaderType& shaderType, const bool optimizeShaders) : m_Device(
    device.GetNativeDevice()) {
    Create(shaderPath, shaderType, optimizeShaders);
}

inline ShaderModule::~ShaderModule() {
    Destroy();
}

inline VkShaderModule ShaderModule::GetNativeShaderModule() const {
    return m_ShaderModule;
}

inline void ShaderModule::Destroy() const {
    if (m_ShaderModule) {
        vkDestroyShaderModule(m_Device, m_ShaderModule, nullptr);
    }
}
