/* Copyright (C) 2024 Jean "Pixfri" Letessier (jean.letessier@protonmail.com)
 * This file is part of "FlashLight Engine"
 * For conditions of distribution and use, see copyright notice in FlashLightEngine.hpp
 *
 * File : VulkanSwapChain.inl
 * Description : This file contains the definition of inline methods from the VulkanSwapChain class.
 */

#pragma once

#include "FlashlightEngine/pch.hpp"

/// @ingroup VulkanRenderer
/// @class Flashlight::VulkanRenderer::VulkanSwapChain
/// @brief VulkanRenderer wrapper class for the Vulkan Swap Chain.

/// @brief Constructor for the VulkanSwapChain class. Calls the function to initialize required objects.
///
/// @param base A VulkanBase instance reference.
inline VulkanSwapChain::VulkanSwapChain(Window &window, VulkanBase &base) : m_Window(window), m_VulkanBase(base) {
    Init();
}

/// @brief Destructor for the VulkanSwapChain class. Calls the function to destroy required objects.
inline VulkanSwapChain::~VulkanSwapChain() {
    Cleanup();
}

/// @brief Creates an instance of this class and returns a std::unique_pointer of it.
///
/// @returns A std::unique_pointer to the created object.
inline std::unique_ptr<VulkanSwapChain> VulkanSwapChain::Create(Window &window, VulkanBase &base) {
    return std::make_unique<VulkanSwapChain>(window, base);
}