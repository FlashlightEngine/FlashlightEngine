#pragma once

#include "FlashlightEngine/Core/FileUtils.hpp"

#include <volk.h>

inline VulkanGraphicsShader::VulkanGraphicsShader() : m_Device(VK_NULL_HANDLE), m_Handle(VK_NULL_HANDLE) {}

inline VulkanGraphicsShader::~VulkanGraphicsShader() {
    if (IsValid()) {
        Destroy();
    }
}

inline bool VulkanGraphicsShader::IsValid() const noexcept {
    return m_Handle != nullptr;
}

inline VkShaderModule VulkanGraphicsShader::GetHandle() const noexcept {
    return m_Handle;
}

inline void VulkanGraphicsShader::Create(const VulkanDevice& device, const std::filesystem::path &shaderPath) {
    auto shaderCode = Flashlight::ReadBinary(shaderPath);

    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = shaderCode.size();
    createInfo.pCode = reinterpret_cast<const u32 *>(shaderCode.data());

    if (vkCreateShaderModule(device.GetHandle(), &createInfo, nullptr, &m_Handle) != VK_SUCCESS) {
        std::cerr << "Failed to create vertex shader module." << std::endl;
        throw std::runtime_error("");
    }

    m_Device = device.GetHandle();
}

inline void VulkanGraphicsShader::Destroy() const {
    if (IsValid()) {
        vkDestroyShaderModule(m_Device, m_Handle, nullptr);
    }
}

