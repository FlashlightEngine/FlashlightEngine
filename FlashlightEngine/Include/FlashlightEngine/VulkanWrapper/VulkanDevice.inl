#pragma once

inline VulkanDevice::VulkanDevice() {}

inline VulkanDevice::~VulkanDevice() {
    Destroy();
}

inline void VulkanDevice::Destroy() const {
    if (IsValid()) {
        DestroyLogicalDevice();
    }
}

inline bool VulkanDevice::IsValid() const noexcept {
    return m_Device != VK_NULL_HANDLE;
}

inline VkDevice VulkanDevice::GetDevice() const noexcept {
    return m_Device;
}