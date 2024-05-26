#pragma once

#include "FlashlightEngine/Core/FileUtils.hpp"

#include <volk.h>

inline VulkanShaderModule::VulkanShaderModule() : m_Device(VK_NULL_HANDLE), m_Handle(VK_NULL_HANDLE) {}

inline VulkanShaderModule::~VulkanShaderModule() {
    if (IsValid()) {
        Destroy();
    }
}

inline bool VulkanShaderModule::IsValid() const noexcept {
    return m_Handle != nullptr;
}

inline VkShaderModule VulkanShaderModule::GetHandle() const noexcept {
    return m_Handle;
}

inline void VulkanShaderModule::Create(const VulkanDevice& device, const std::filesystem::path &shaderPath) {
    auto shaderCode = Flashlight::ReadBinary(shaderPath);

    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = shaderCode.size();
    createInfo.pCode = reinterpret_cast<const u32 *>(shaderCode.data());

    if (vkCreateShaderModule(device.GetHandle(), &createInfo, nullptr, &m_Handle) != VK_SUCCESS) {
        FL_ERROR("Failed to create shader module.")
    }

    m_Device = device.GetHandle();
}

inline void VulkanShaderModule::Destroy() const {
    if (IsValid()) {
        vkDestroyShaderModule(m_Device, m_Handle, nullptr);
    }
}

