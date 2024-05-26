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

inline std::vector<VkImage> VulkanSwapChain::GetSwapChainImages() const noexcept {
    return m_SwapChainImages;
}

inline VkImageView VulkanSwapChain::GetNativeImageViewAtIndex(const i32 index) const noexcept {
    return m_SwapChainImageViews[index].GetHandle();
}

inline VkFormat VulkanSwapChain::GetSwapChainImageFormat() const noexcept {
    return m_SwapChainImageFormat;
}

inline VkExtent2D VulkanSwapChain::GetSwapChainExtent() const noexcept {
    return m_SwapChainExtent;
}