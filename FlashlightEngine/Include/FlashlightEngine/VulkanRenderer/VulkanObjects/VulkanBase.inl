#pragma once

#include "VulkanBase.hpp"

/// @ingroup VulkanRenderer
/// @class Flashlight::VulkanBase
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

/// @brief Creates an instance of this class and returns a std::unqiue_pointer of it.
///
/// @returns A std::unique_pointer to the created object.
inline std::unique_ptr<VulkanBase> VulkanBase::Create(const Window &window) {
    return std::make_unique<VulkanBase>(window);
}

/// @brief Returns the logical device.
///
/// @returns The VkDevice handle of the logical device.
inline VkDevice VulkanBase::Device() noexcept {
    return m_Vulkan.LogicalDevice;
}

/// @brief Returns the window surface.
///
/// @returns The VkSurfaceKHR handle of the window surface.
inline VkSurfaceKHR VulkanBase::Surface() noexcept {
    return m_Vulkan.Surface;
}

/// @brief Returns the graphics queue.
///
/// @returns The VkQueue handle of the graphics queue.
inline VkQueue VulkanBase::GraphicsQueue() noexcept {
    return m_Vulkan.GraphicsQueue;
}

/// @brief Returns the present queue.
///
/// @returns The VkQueue handle of the present queue.
inline VkQueue VulkanBase::PresentQueue() noexcept {
    return m_Vulkan.PresentQueue;
}

/// @brief Returns the swap chain support details.
///
/// @returns A SwapChainSupportDetails struct of the support details for the swap chain.
inline SwapChainSupportDetails VulkanBase::GetSwapChainSupportDetails() noexcept {
    SwapChainSupportDetails supportDetails = QuerySwapChainSupport(m_Vulkan.PhysicalDevice);

    return supportDetails;
}

/// @brief Returns the queue families indices.
///
/// @returns A QueueFamilyIndices struct of the queue families .
inline QueueFamilyIndices VulkanBase::GetQueueFamilies() noexcept {
    QueueFamilyIndices indices = FindQueueFamilies(m_Vulkan.PhysicalDevice);

    return indices;
}