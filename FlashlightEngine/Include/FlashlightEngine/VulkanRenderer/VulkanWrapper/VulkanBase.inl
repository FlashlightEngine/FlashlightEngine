/* Copyright (C) 2024 Jean "Pixfri" Letessier (jean.letessier@protonmail.com)
 * This file is part of "FlashLight Engine"
 * For conditions of distribution and use, see copyright notice in FlashLightEngine.hpp
 *
 * File : VulkanBase.inl
 * Description : This file contains the definitions of inline methods from the VulkanBase class.
 */

#pragma once

/// @ingroup VulkanRenderer
/// @class Flashlight::VulkanWrapper::VulkanBase
/// @brief VulkanRenderer wrapper class for base Vulkan objects.

/// @brief Constructor for the VulkanBase class. Calls the function to initialize
/// every Vulkan object.
///
/// @param window The window of the application using the renderer.
inline VulkanBase::VulkanBase(const Window &window) : m_Window(window) {
    Init();
}

/// @brief Destructor of the VulkanBase class. Destroys native Vulkan
/// objects created in this class.
inline VulkanBase::~VulkanBase() {
    Cleanup();
}

/// @brief Creates an instance of this class and returns a std::unique_pointer of it.
///
/// @returns A std::unique_pointer to the created object.
inline std::unique_ptr<VulkanBase> VulkanBase::Create(const Window &window) {
    return std::make_unique<VulkanBase>(window);
}

/// @brief Returns the logical device.
///
/// @returns The VkDevice handle of the logical device.
inline VkDevice VulkanBase::Device() const noexcept {
    return m_Vulkan.LogicalDevice;
}

/// @brief Returns the window surface.
///
/// @returns The VkSurfaceKHR handle of the window surface.
inline VkSurfaceKHR VulkanBase::Surface() const noexcept {
    return m_Vulkan.Surface;
}

/// @brief Returns the graphics queue.
///
/// @returns The VkQueue handle of the graphics queue.
inline VkQueue VulkanBase::GraphicsQueue() const noexcept {
    return m_Vulkan.GraphicsQueue;
}

/// @brief Returns the present queue.
///
/// @returns The VkQueue handle of the present queue.
inline VkQueue VulkanBase::PresentQueue() const noexcept {
    return m_Vulkan.PresentQueue;
}

/// @brief Returns the swap chain support details.
///
/// @returns A SwapChainSupportDetails struct of the support details for the swap chain.
inline SwapChainSupportDetails VulkanBase::GetSwapChainSupportDetails() const noexcept {
    return QuerySwapChainSupport(m_Vulkan.PhysicalDevice);
}

/// @brief Returns the queue families indices.
///
/// @returns A QueueFamilyIndices struct of the queue families .
inline QueueFamilyIndices VulkanBase::GetQueueFamilies() const noexcept {
    return FindQueueFamilies(m_Vulkan.PhysicalDevice);
}