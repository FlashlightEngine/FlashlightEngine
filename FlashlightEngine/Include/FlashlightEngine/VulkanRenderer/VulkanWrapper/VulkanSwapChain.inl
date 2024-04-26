#pragma once

inline VulkanSwapChain::VulkanSwapChain() : m_Device(VK_NULL_HANDLE), m_Handle(VK_NULL_HANDLE) {}

inline VulkanSwapChain::~VulkanSwapChain() {
    if (IsValid()) {
        Destroy();
    }
}

inline void VulkanSwapChain::Destroy() const {
    if (IsValid()) {
        vkDestroySwapchainKHR(m_Device, m_Handle, nullptr);
    }
}

inline bool VulkanSwapChain::IsValid() const noexcept {
    return m_Handle != VK_NULL_HANDLE;
}

inline VkSwapchainKHR VulkanSwapChain::GetHandle() const noexcept {
    if (!IsValid()) {
        return nullptr;
    }

    return m_Handle;
}
