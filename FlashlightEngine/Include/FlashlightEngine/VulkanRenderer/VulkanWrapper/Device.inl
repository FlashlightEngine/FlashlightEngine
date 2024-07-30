/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Device.inl
 * Description : Definitions of inline methods from the Device class.
 */
#pragma once

inline VkPhysicalDevice Device::GetPhysicalDevice() const {
    return m_PhysicalDevice;
}

inline VkDevice Device::GetLogicalDevice() const {
    return m_Device;
}

inline VkQueue Device::GetGraphicsQueue() const {
    return m_GraphicsQueue;
}

inline u32 Device::GetGraphicsQueueFamilyIndex() const {
    return m_GraphicsQueueFamily;
}

inline VkQueue Device::GetPresentQueue() const {
    return m_PresentQueue;
}

inline u32 Device::GetPresentQueueFamilyIndex() const {
    return m_PresentQueueFamily;
}
