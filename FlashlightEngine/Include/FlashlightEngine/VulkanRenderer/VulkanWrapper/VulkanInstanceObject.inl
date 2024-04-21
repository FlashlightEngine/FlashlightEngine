/* Copyright (C) 2024 Jean "Pixfri" Letessier (jean.letessier@protonmail.com)
* This file is part of "FlashLight Engine"
 * For conditions of distribution and use, see copyright notice in FlashLightEngine.hpp
 *
 * File : VulkanObject.inl
 * Description : This file contains the definitions of inline methods from the VulkanObject class.
 */

#pragma once

template<typename ClassName, typename VkType, typename VkCreateInfo>
VulkanInstanceObject<ClassName, VkType, VkCreateInfo>::VulkanInstanceObject() : m_Handle(VK_NULL_HANDLE) {
}

template<typename ClassName, typename VkType, typename VkCreateInfo>
VulkanInstanceObject<ClassName, VkType, VkCreateInfo>::~VulkanInstanceObject() {
    Destroy();
}

template<typename ClassName, typename VkType, typename VkCreateInfo>
bool VulkanInstanceObject<ClassName, VkType, VkCreateInfo>::IsValid() const noexcept {
    return m_Handle != VK_NULL_HANDLE;
}

template <typename ClassName, typename VkType, typename VkCreateInfoType>
void VulkanInstanceObject<ClassName, VkType, VkCreateInfoType>::Destroy() {
    if (IsValid()) {
        ClassName::DestroyHandle(m_Instance.get(), m_Handle);
    }
}
