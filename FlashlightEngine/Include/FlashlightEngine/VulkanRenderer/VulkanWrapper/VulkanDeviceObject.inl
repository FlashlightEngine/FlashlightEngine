/* Copyright (C) 2024 Jean "Pixfri" Letessier (jean.letessier@protonmail.com)
* This file is part of "FlashLight Engine"
 * For conditions of distribution and use, see copyright notice in FlashLightEngine.hpp
 *
 * File : VulkanObject.inl
 * Description : This file contains the definitions of inline methods from the VulkanObject class.
 */

#pragma once

template<typename VkType>
inline VulkanDeviceObject<VkType>::VulkanDeviceObject(const VulkanBase &base) : m_Device{base.Device()} {
    Init();
}

template<typename VkType>
inline VulkanDeviceObject<VkType>::~VulkanDeviceObject() {
    Cleanup();
}

template<typename VkType>
inline bool VulkanDeviceObject<VkType>::IsValid() const noexcept {
    return m_Handle != nullptr;
}
