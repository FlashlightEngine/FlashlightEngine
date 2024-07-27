/* Copyright (C) 2024 Jean "Pixfri" Letessier
* This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Device.inl
 * Description : Definitions of inline methods from the Device class.
 */
#pragma once

inline Device::Device(const std::shared_ptr<Instance>& instance, Surface& surface,
                      const DebugLevel& debugLevel) : m_Instance(instance),
                                                      m_ValidationLayers(instance->GetValidationLayers()),
                                                      m_Surface(surface) {
    PickPhysicalDevice();
    CreateLogicalDevice(debugLevel);
    CreateSingleTimeCommandsPool();
}

inline Device::~Device() {
    if (m_SingleTimeCommandPool) {
        Log::EngineTrace("Destroying single time command pool.");
        vkDestroyCommandPool(m_Device, m_SingleTimeCommandPool, nullptr);
    }

    if (m_Device) {
        Log::EngineTrace("Destroying Vulkan device.");
        vkDestroyDevice(m_Device, nullptr);
    }
}

#pragma region Physical Device
inline VkPhysicalDevice Device::GetNativePhysicalDevice() const {
    return m_PhysicalDevice;
}

inline VkPhysicalDeviceProperties Device::GetPhysicalDeviceProperties() const {
    return m_PhysicalDeviceProperties;
}

inline VkPhysicalDeviceFeatures Device::GetPhysicalDeviceFeatures() const {
    return m_PhysicalDeviceFeatures;
}

inline QueueFamilyIndices Device::GetQueueFamilies() const {
    return m_QueueFamilies;
}

inline SwapChainSupportDetails Device::GetSwapChainSupport() const {
    return QuerySwapChainSupport(m_PhysicalDevice);
}
#pragma endregion

#pragma region Logical Device
inline VkDevice Device::GetNativeDevice() const {
    return m_Device;
}

inline VkQueue Device::GetGraphicsQueue() const {
    return m_GraphicsQueue;
}

inline VkQueue Device::GetPresentQueue() const {
    return m_PresentQueue;
}
#pragma endregion
