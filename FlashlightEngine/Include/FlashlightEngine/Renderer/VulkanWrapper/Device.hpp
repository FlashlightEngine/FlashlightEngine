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

        VkDevice m_Device = VK_NULL_HANDLE;
        VkQueue m_GraphicsQueue = VK_NULL_HANDLE;
        VkQueue m_PresentQueue = VK_NULL_HANDLE;

        std::vector<const char*> m_DeviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };

        VkCommandPool m_SingleTimeCommandPool = VK_NULL_HANDLE;

        Instance& m_Instance;
        std::vector<const char*> m_ValidationLayers;
        Surface& m_Surface;
    
    public:
        inline Device(Instance& instance, Surface& surface, const DebugLevel& debugLevel);
        inline ~Device();

        Device(const Device&) = delete;
        Device(Device&&) = delete;

        Device& operator=(const Device&) = delete;
        Device& operator=(Device&&) = delete;

#pragma region Physical Device
        [[nodiscard]] inline VkPhysicalDevice GetNativePhysicalDevice() const;
        [[nodiscard]] inline VkPhysicalDeviceProperties GetPhysicalDeviceProperties() const;
        [[nodiscard]] inline VkPhysicalDeviceFeatures GetPhysicalDeviceFeatures() const;
        [[nodiscard]] inline QueueFamilyIndices GetQueueFamilies() const;
        [[nodiscard]] inline SwapChainSupportDetails GetSwapChainSupport() const;
#pragma endregion

#pragma region Logical Device 
        [[nodiscard]] inline VkDevice GetNativeDevice() const;
        [[nodiscard]] inline VkQueue GetGraphicsQueue() const;
        [[nodiscard]] inline VkQueue GetPresentQueue() const;
#pragma endregion

        [[nodiscard]] u32 FindMemoryType(u32 typeFilter, VkMemoryPropertyFlags properties) const;
        void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) const;

    private:
        void PickPhysicalDevice();
        void CreateLogicalDevice(const DebugLevel& debugLevel);
        void CreateSingleTimeCommandsPool();

#pragma region Physical Device Utility
        [[nodiscard]] int RateDeviceSuitability(VkPhysicalDevice physicalDevice);
        [[nodiscard]] bool IsDeviceSuitable(VkPhysicalDevice physicalDevice);
        [[nodiscard]] static VkPhysicalDeviceProperties
        GetPhysicalDeviceProperties(VkPhysicalDevice physicalDevice);
        [[nodiscard]] static VkPhysicalDeviceFeatures GetPhysicalDeviceFeatures(VkPhysicalDevice physicalDevice);
        [[nodiscard]] QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice physicalDevice) const;
        [[nodiscard]] bool CheckDeviceExtensionsSupport(VkPhysicalDevice physicalDevice);
        [[nodiscard]] static std::vector<VkExtensionProperties> GetAvailableDeviceExtensions(VkPhysicalDevice physicalDevice);
        [[nodiscard]] SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice physicalDevice) const;
#pragma endregion
        
        // Logical device utility functions.
    };

#include "Device.inl"
}
