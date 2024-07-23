/* Copyright (C) 2024 Jean "Pixfri" Letessier
* This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Device.inl
 * Description : Definitions of inline methods from the Device class.
 */
#pragma once

inline
Device::Device(const Instance& instance, const Surface& surface,
               const DebugLevel& debugLevel) : m_Instance(instance.GetNativeInstance()),
                                               m_ValidationLayers(instance.GetValidationLayers()),
                                               m_Surface(surface.GetNativeSurface()) {
    PickPhysicalDevice();
    CreateLogicalDevice(debugLevel);
}

inline Device::~Device() {
    Destroy();
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
    return m_SwapChainSupport;
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

inline void Device::Destroy() const {
    if (m_Device) {
        Log::EngineTrace("Destroying Vulkan device.");
        vkDestroyDevice(m_Device, nullptr);
    }
}
