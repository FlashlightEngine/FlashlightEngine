/* Copyright (C) 2024 Jean "Pixfri" Letessier
* This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Device.inl
 * Description : Definitions of inline methods from the Device class.
 */
#pragma once

inline Device::Device(const Instance& instance, const DebugLevel& debugLevel) {
    m_Instance = instance.GetNativeInstance();
    m_ValidationLayers = instance.GetValidationLayers();
    
    PickPhysicalDevice();
    CreateLogicalDevice(debugLevel);
}

inline Device::~Device() {
    Destroy();
}

inline VkPhysicalDevice Device::GetNativePhysicalDevice() const {
    return m_PhysicalDevice;
}

inline VkDevice Device::GetNativeDevice() const {
    return m_Device;
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

inline void Device::Destroy() const {
    if (m_Device) {
        vkDestroyDevice(m_Device, nullptr);
    }
}
