#pragma once

VulkanPhysicalDevice::VulkanPhysicalDevice() : m_PhysicalDevice(VK_NULL_HANDLE) {}

inline bool VulkanPhysicalDevice::IsValid() const noexcept {
    return m_PhysicalDevice != VK_NULL_HANDLE;
}

inline VkPhysicalDevice VulkanPhysicalDevice::GetPhysicalDevice() const noexcept {
    return m_PhysicalDevice;
}

inline VkPhysicalDeviceProperties VulkanPhysicalDevice::GetPhysicalDeviceProperties() const noexcept {
    return GetPhysicalDeviceProperties(m_PhysicalDevice);
}

inline VkPhysicalDeviceFeatures VulkanPhysicalDevice::GetPhysicalDeviceFeatures() const noexcept {
    return GetPhysicalDeviceFeatures(m_PhysicalDevice);
}

inline std::vector<VkExtensionProperties> VulkanPhysicalDevice::GetAvailableExtensions() const noexcept {
    return GetAvailableExtensions(m_PhysicalDevice);
}

inline QueueFamilyIndices VulkanPhysicalDevice::FindQueueFamilies() const noexcept {
    return FindQueueFamilies(m_PhysicalDevice);
}

inline SwapChainSupportDetails VulkanPhysicalDevice::GetSwapChainSupport() const noexcept {
    return QuerySwapChainSupport(m_PhysicalDevice);
}

inline std::vector<const char*> VulkanPhysicalDevice::GetRequiredExtensions() const noexcept {
    return m_DeviceExtensions;
}

inline VkPhysicalDeviceProperties VulkanPhysicalDevice::GetPhysicalDeviceProperties(VkPhysicalDevice physicalDevice) const noexcept {
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(physicalDevice, &properties);

    return properties;
}

inline VkPhysicalDeviceFeatures VulkanPhysicalDevice::GetPhysicalDeviceFeatures(VkPhysicalDevice physicalDevice) const noexcept {
    VkPhysicalDeviceFeatures features;
    vkGetPhysicalDeviceFeatures(physicalDevice, &features);

    return features;
}

inline std::vector<VkExtensionProperties> VulkanPhysicalDevice::GetAvailableExtensions(VkPhysicalDevice physicalDevice) const noexcept {
    u32 extensionCount = 0;
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount,  availableExtensions.data());

    return availableExtensions;
}