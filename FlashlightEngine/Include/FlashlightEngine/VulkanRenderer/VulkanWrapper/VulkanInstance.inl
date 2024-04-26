/* Copyright (C) 2024 Jean "Pixfri" Letessier (jean.letessier@protonmail.com)
 * This file is part of "FlashLight Engine"
 * For conditions of distribution and use, see copyright notice in FlashLightEngine.hpp
 *
 * File : VulkanInstance.inl
 * Description : This file contains the definitions of inline methods from the VulkanInstance class.
 */

#pragma once

#include <volk.h>

/// @ingroup VulkanRenderer
/// @class Flashlight::VulkanWrapper::VulkanInstance
/// @brief Vulkan wrapper class that handles the Vulkan instance.

/// @brief Constructor for the VulkanInstance class. Initializes the VkInstance handle to VK_NULL_HANDLE.
inline VulkanInstance::VulkanInstance() : m_Handle(VK_NULL_HANDLE) {
}

/// @brief Destructor for the VulkanInstance class. Destroys the VkInstance handle.
inline VulkanInstance::~VulkanInstance() {
    Cleanup();
}

/// @brief Returns the enabled validations layers.
///
/// @returns A vector containing the names of enabled validation layers.
inline std::vector<const char*> VulkanInstance::GetEnabledValidationLayers() const noexcept {
    return m_ValidationLayers;
}

/// @brief Checks if the VkInstance handle is valid.
///
/// @returns A boolean telling if the VkInstance handle is valid.
inline bool VulkanInstance::IsValid() const noexcept {
    return m_Handle != nullptr;
}

inline VkInstance VulkanInstance::GetHandle() const noexcept {
    return m_Handle;
}