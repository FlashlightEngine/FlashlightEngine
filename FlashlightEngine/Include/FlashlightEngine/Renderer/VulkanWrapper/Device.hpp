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

#include "FlashlightEngine/pch.hpp"

#include <volk.h>

namespace Flashlight::VulkanWrapper {
    struct QueueFamilyIndices {
        u32 GraphicsFamily;
        bool FoundGraphicsFamily;

        [[nodiscard]]
        bool IsComplete() const {
            return FoundGraphicsFamily;
        }
    };

    class Device {
        VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
        VkPhysicalDeviceProperties m_PhysicalDeviceProperties;
        VkPhysicalDeviceFeatures m_PhysicalDeviceFeatures;
        QueueFamilyIndices m_QueueFamilies;

        VkDevice m_Device = VK_NULL_HANDLE;

        VkInstance m_Instance = VK_NULL_HANDLE;
        std::vector<const char*> m_ValidationLayers;

    public:
        inline Device(const Instance& instance, const DebugLevel& debugLevel);
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

    private:
        void PickPhysicalDevice();
        void CreateLogicalDevice(const DebugLevel& debugLevel);

        inline void Destroy() const;

        // Physical device utility functions.
        [[nodiscard]] static int RateDeviceSuitability(VkPhysicalDevice physicalDevice);
        [[nodiscard]] static bool IsDeviceSuitable(VkPhysicalDevice physicalDevice);
        [[nodiscard]] static VkPhysicalDeviceProperties
        GetPhysicalDeviceProperties(VkPhysicalDevice physicalDevice);
        [[nodiscard]] static VkPhysicalDeviceFeatures GetPhysicalDeviceFeatures(VkPhysicalDevice physicalDevice);
        [[nodiscard]] static QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice physicalDevice);

        // Logical device utility functions.
    };

#include "Device.inl"
}
