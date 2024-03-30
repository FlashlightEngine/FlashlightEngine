/* Copyright (C) 2024 Jean "Pixfri" Letessier (jean.letessier@protonmail.com)
 * This file is part of "FlashLight Engine"
 * For conditions of distribution and use, see copyright notice in FlashLightEngine.hpp
 *
 * VulkanBase.hpp - The definition of the VulkanBase class.
 * This header file contains the definition of the VulkanBase class, which is a wrapper for Vulkan base objects (instance, device, etc.).
 */

#pragma once

#include "FlashlightEngine/pch.hpp"
#include "FlashlightEngine/Core/Window.hpp"

#include <volk.h>

namespace Flashlight {

/// @ingroup VulkanRenderer
/// @struct Flashlight::VulkanBaseObjects
/// @brief Structure that stores base Vulkan objects.
struct VulkanBaseObjects {
    VkInstance Instance;

    VkDebugUtilsMessengerEXT DebugMessenger;

    VkPhysicalDevice PhysicalDevice;

    VkDevice LogicalDevice;

    VkQueue GraphicsQueue;
    VkQueue PresentQueue;

    VkSurfaceKHR Surface;

    /// @brief Constructor for the VulkanBaseObjects struct.
    /// Initalizes every object to VK_NULL_HANDLE by default.
    VulkanBaseObjects() :
        Instance(VK_NULL_HANDLE),
        DebugMessenger(VK_NULL_HANDLE),
        PhysicalDevice(VK_NULL_HANDLE),
        LogicalDevice(VK_NULL_HANDLE),
        GraphicsQueue(VK_NULL_HANDLE),
        PresentQueue(VK_NULL_HANDLE),
        Surface(VK_NULL_HANDLE) {}
};

/// @ingroup VulkanRenderer
/// @struct Flashlight::QueueFamilyIndices
/// @brief Structure that stores queue family indicies and a utility function to check if they are both present.
struct QueueFamilyIndices {
    u32 GraphicsFamily = 0;
    u32 PresentFamily = 0;
    bool GraphicsFamilyHasValue = false;
    bool PresentFamilyHasValue = false;

    /// @brief Function to check if every queue family is set.
    ///
    /// @returns A boolean telling if every queue family has a value.
    bool IsComplete() {
        return GraphicsFamilyHasValue && PresentFamilyHasValue;
    }
};

/// @ingroup VulkanRenderer
/// @struct Flashlight::SwapChainSupportDetails
/// @brief Struct that stores properties for the swap chain.
struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR Capabilities;
    std::vector<VkSurfaceFormatKHR> Formats;
    std::vector<VkPresentModeKHR> PresentModes;
};

class VulkanBase {
public:
    inline explicit VulkanBase(const Window &window);
    inline ~VulkanBase();

    // We can't copy/move vulkan objects.
    VulkanBase(const VulkanBase &) = delete;
    VulkanBase &operator=(const VulkanBase &) = delete;
    VulkanBase(VulkanBase &&) = delete;
    VulkanBase &operator=(VulkanBase &&) = delete;

    inline static std::unique_ptr<VulkanBase> Create(const Window &window);

    // Getter functions.
    inline VkDevice Device() const noexcept;
    inline VkSurfaceKHR Surface() const noexcept;
    inline VkQueue GraphicsQueue() const noexcept;
    inline VkQueue PresentQueue() const noexcept;

    inline SwapChainSupportDetails GetSwapChainSupportDetails() const noexcept;
    inline QueueFamilyIndices GetQueueFamilies() const noexcept;

    VkPhysicalDeviceProperties DeviceProperties;

private:
    Window m_Window;
    VulkanBaseObjects m_Vulkan;

    void Init();
    void Cleanup();

    // Functions to create/destroy Vulkan objects.
    void CreateInstance();
    void DestroyInstance() const;

    void CreateDebugMessenger();
    void DestroyDebugMessenger() const;

    void CreateSurface();
    void DestroySurface() const;

    // The physical device doesn't need a destroy function since it's destroyed implicitly when the instance is destroyed.
    void PickPhysicalDevice();

    void CreateLogicalDevice();
    void DestroyLogicalDevice() const;

    // Utility functions.
    std::vector<const char*> GetRequiredInstanceExtensions() const noexcept;
    std::vector<VkExtensionProperties> GetAvailableInstanceExtensions() const noexcept;
    void CheckRequiredInstanceExtensionsSupport() const;
    bool CheckValidationLayerSupport() const noexcept;
    void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo) noexcept;
    VkPhysicalDeviceProperties GetDeviceProperties(VkPhysicalDevice physicalDevice) const noexcept;
    VkPhysicalDeviceFeatures GetDeviceFeatures(VkPhysicalDevice physicalDevice) const noexcept;
    bool IsDeviceSuitable(VkPhysicalDevice physicalDevice) const noexcept;
    bool CheckDeviceExtensionSupport(VkPhysicalDevice physicalDevice) const noexcept;
    std::vector<VkExtensionProperties> GetAvailableDeviceExtensions(VkPhysicalDevice physicalDevice) const noexcept;
    int RateDeviceSuitability(VkPhysicalDevice physicalDevice) const noexcept;
    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice physicalDevice) const noexcept;
    SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice physicalDevice) const noexcept;

    const std::vector<const char*> m_ValidationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };

    const std::vector<const char*> m_DeviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
};

#include "VulkanBase.inl"

}
