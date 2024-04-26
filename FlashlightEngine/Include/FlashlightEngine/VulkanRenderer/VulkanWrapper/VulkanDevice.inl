#pragma once

inline VulkanDevice::VulkanDevice() {}

inline VulkanDevice::~VulkanDevice() {
    if (IsValid()) {
        Destroy();
    }
}

inline void VulkanDevice::Destroy() const {
    if (IsValid()) {
        DestroyLogicalDevice();
    }
}

inline bool VulkanDevice::IsValid() const noexcept {
    return m_PhysicalDevice != VK_NULL_HANDLE && m_Device != VK_NULL_HANDLE;
}

inline VkDevice VulkanDevice::GetHandle() const noexcept {
    if (!IsValid()) {
        return nullptr;
    }

    return m_Device;
}

inline QueueFamilyIndices VulkanDevice::FindQueueFamilies() const noexcept {
    return FindQueueFamilies(m_PhysicalDevice);
}

inline SwapChainSupportDetails VulkanDevice::QuerySwapChainSupport() const noexcept {
    return QuerySwapChainSupport(m_PhysicalDevice);
}

inline VkPhysicalDeviceProperties VulkanDevice::GetDeviceProperties() const noexcept {
    return m_DeviceProperties;
}

inline VkPhysicalDeviceFeatures VulkanDevice::GetDeviceFeatures() const noexcept {
    return m_DeviceFeatures;
}

/// @brief Gets a physical device properties.
///
/// @param physicalDevice The physical device to get the properties of.
///
/// @returns The physical device properties.
inline VkPhysicalDeviceProperties VulkanDevice::GetDeviceProperties(VkPhysicalDevice physicalDevice) const noexcept {
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);

    return deviceProperties;
}

/// @brief Gets a physical device features.
///
/// @param physicalDevice The physical device to get the properties of.
///
/// @returns The physical device features.
inline VkPhysicalDeviceFeatures VulkanDevice::GetDeviceFeatures(VkPhysicalDevice physicalDevice) const noexcept {
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);

    return deviceFeatures;
}

/// @brief Returns available extensions for the provided device.
///
/// @param physicalDevice The physical device to check.
///
/// @returns A std::vector of the extensions supported by the device.
inline std::vector<VkExtensionProperties> VulkanDevice::GetAvailableDeviceExtensions(VkPhysicalDevice physicalDevice) const noexcept {
    u32 extensionCount = 0;
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableExtensions.data());

    return availableExtensions;
}