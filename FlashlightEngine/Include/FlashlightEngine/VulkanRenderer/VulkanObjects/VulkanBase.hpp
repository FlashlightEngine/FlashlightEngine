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

    /// @brief Constructor for the VulkanBaseObjects struct.
    /// Initalizes every object to VK_NULL_HANDLE by default.
    VulkanBaseObjects() :
        Instance(VK_NULL_HANDLE),
        DebugMessenger(VK_NULL_HANDLE),
        PhysicalDevice(VK_NULL_HANDLE) {}
};

/// @ingroup VulkanRenderer
/// @struct Flashlight::QueueFamilyIndices
/// @brief Structure that stores queue family indicies and a utility function to check if they are both present.
struct QueueFamilyIndices {
    uint32 GraphicsFamily = 0;
    bool GraphicsFamilyHasValue;

    /// @brief Function to check if every queue family is set.
    ///
    /// @returns A boolean telling if every queue family has a value.
    bool IsComplete() {
        return GraphicsFamilyHasValue;
    }
};

class VulkanBase {
public:
    explicit VulkanBase(const Window &window);
    ~VulkanBase();

    // We can't copy/move vulkan objects.
    VulkanBase(const VulkanBase &) = delete;
    VulkanBase &operator=(const VulkanBase &) = delete;
    VulkanBase(VulkanBase &&) = delete;
    VulkanBase &operator=(VulkanBase &&) = delete;

    static std::unique_ptr<VulkanBase> Create(const Window &window);

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

    // The physical device doesn't need a destroy function since it's destroyed implicitly when the instance is destroyed.
    void PickPhysicalDevice();

    // Utility functions.
    std::vector<const char*> GetRequiredInstanceExtensions() const noexcept;
    std::vector<VkExtensionProperties> GetAvailableInstanceExtensions() const noexcept;
    bool CheckRequiredInstanceExtensionsSupport() const noexcept;
    bool CheckValidationLayerSupport() const noexcept;
    void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo) noexcept;
    VkPhysicalDeviceProperties GetDeviceProperties(VkPhysicalDevice physicalDevice) const noexcept;
    VkPhysicalDeviceFeatures GetDeviceFeatures(VkPhysicalDevice physicalDevice) const noexcept;
    bool IsDeviceSuitable(VkPhysicalDevice physicalDevice) const noexcept;
    int RateDeviceSuitability(VkPhysicalDevice physicalDevice) const noexcept;
    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice physicalDevice) const noexcept;

    const std::vector<const char*> m_validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };

#ifdef NDEBUG
    const bool m_EnableValidationLayers = false;
#else
    const bool m_EnableValidationLayers = true;
#endif
};

}
