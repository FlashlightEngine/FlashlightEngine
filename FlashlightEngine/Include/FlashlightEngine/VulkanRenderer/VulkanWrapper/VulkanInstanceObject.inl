/* Copyright (C) 2024 Jean "Pixfri" Letessier (jean.letessier@protonmail.com)
* This file is part of "FlashLight Engine"
 * For conditions of distribution and use, see copyright notice in FlashLightEngine.hpp
 *
 * File : VulkanObject.inl
 * Description : This file contains the definitions of inline methods from the VulkanObject class.
 */

#pragma once

template<typename ClassName, typename VkType, typename VkCreateInfoType>
VulkanInstanceObject<ClassName, VkType, VkCreateInfoType>::VulkanInstanceObject() : m_Handle(VK_NULL_HANDLE) {
}

template<typename ClassName, typename VkType, typename VkCreateInfoType>
VulkanInstanceObject<ClassName, VkType, VkCreateInfoType>::~VulkanInstanceObject() {
    Destroy();
}

template<typename ClassName, typename VkType, typename VkCreateInfoType>
bool VulkanInstanceObject<ClassName, VkType, VkCreateInfoType>::IsValid() const noexcept {
    return m_Handle != VK_NULL_HANDLE;
}

template<typename ClassName, typename VkType, typename VkCreateInfoType>
VkType VulkanInstanceObject<ClassName, VkType, VkCreateInfoType>::GetHandle() const noexcept {
    return m_Handle;
}

template <typename ClassName, typename VkType, typename VkCreateInfoType>
void VulkanInstanceObject<ClassName, VkType, VkCreateInfoType>::Create(VulkanInstance &instance, VkCreateInfoType &createInfo) {
    m_Instance = &instance;
    ClassName::CreateHandle(&m_Instance, createInfo, &m_Handle);
}


template <typename ClassName, typename VkType, typename VkCreateInfoType>
void VulkanInstanceObject<ClassName, VkType, VkCreateInfoType>::Destroy() {
    if (IsValid()) {
        ClassName::DestroyHandle(&m_Instance, &m_Handle);
    }
}
