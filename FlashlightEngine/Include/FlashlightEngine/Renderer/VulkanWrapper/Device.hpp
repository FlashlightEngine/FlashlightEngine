/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Device.hpp
 * Description : Declaration of the Device class. It is used to handle the physical and logical device in Vulkan.
 *               The physical device represents a single complete implementation of Vulkan. A logical device is an
 *               instance of this implementation with its own resources and objects independent from other devices.
 */
#pragma once

#include "FlashlightEngine/Renderer/VulkanWrapper/Instance.hpp"
#include "FlashlightEngine/Renderer/VulkanWrapper/Surface.hpp"

#include "FlashlightEngine/pch.hpp"

namespace Flashlight::VulkanWrapper {
    struct QueueFamilyIndices {
        u32 GraphicsFamily;
        bool FoundGraphicsFamily;
        u32 PresentFamily;
        bool FoundPresentFamily;

        [[nodiscard]]
        bool IsComplete() const {
            return FoundGraphicsFamily && FoundPresentFamily;
        }
    };

    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR Capabilities;
        std::vector<VkSurfaceFormatKHR> Formats;
        std::vector<VkPresentModeKHR> PresentModes;
    };
    
    class Device {
        VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
        VkPhysicalDeviceProperties m_PhysicalDeviceProperties;
        VkPhysicalDeviceFeatures m_PhysicalDeviceFeatures;
        QueueFamilyIndices m_QueueFamilies;
        SwapChainSupportDetails m_SwapChainSupport;

        VkDevice m_Device = VK_NULL_HANDLE;
        VkQueue m_GraphicsQueue = VK_NULL_HANDLE;
        VkQueue m_PresentQueue = VK_NULL_HANDLE;

        std::vector<const char*> m_DeviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };

        VkInstance m_Instance = VK_NULL_HANDLE;
        std::vector<const char*> m_ValidationLayers;
        VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
    
    public:
        inline Device(const Instance& instance, const Surface& surface, const DebugLevel& debugLevel);
        inline ~Device();

        Device(const Device&) = delete;
        Device(Device&&) = delete;

        Device& operator=(const Device&) = delete;
        Device& operator=(Device&&) = delete;

        [[nodiscard]] inline VkPhysicalDevice GetNativePhysicalDevice() const;
        [[nodiscard]] inline VkDevice GetNativeDevice() const;
        [[nodiscard]] inline VkPhysicalDeviceProperties GetPhysicalDeviceProperties() const;
        [[nodiscard]] inline VkPhysicalDeviceFeatures GetPhysicalDeviceFeatures() const;
        [[nodiscard]] inline QueueFamilyIndices GetQueueFamilies() const;
        [[nodiscard]] inline SwapChainSupportDetails GetSwapChainSupport() const;
        [[nodiscard]] inline VkQueue GetGraphicsQueue() const;
        [[nodiscard]] inline VkQueue GetPresentQueue() const;

    private:
        void PickPhysicalDevice();
        void CreateLogicalDevice(const DebugLevel& debugLevel);

        inline void Destroy() const;

        // Physical device utility functions.
        [[nodiscard]] int RateDeviceSuitability(VkPhysicalDevice physicalDevice);
        [[nodiscard]] bool IsDeviceSuitable(VkPhysicalDevice physicalDevice);
        [[nodiscard]] static VkPhysicalDeviceProperties
        GetPhysicalDeviceProperties(VkPhysicalDevice physicalDevice);
        [[nodiscard]] static VkPhysicalDeviceFeatures GetPhysicalDeviceFeatures(VkPhysicalDevice physicalDevice);
        [[nodiscard]] QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice physicalDevice) const;
        [[nodiscard]] bool CheckDeviceExtensionsSupport(VkPhysicalDevice physicalDevice);
        [[nodiscard]] static std::vector<VkExtensionProperties> GetAvailableDeviceExtensions(VkPhysicalDevice physicalDevice);
        [[nodiscard]] SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice physicalDevice) const;
        
        // Logical device utility functions.
    };

#include "Device.inl"
}
