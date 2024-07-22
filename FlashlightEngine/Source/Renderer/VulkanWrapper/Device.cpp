/* Copyright (C) 2024 Jean "Pixfri" Letessier
* This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Device.cpp
 * Description : Definitions of methods from the Device class.
 */
#include "FlashlightEngine/Renderer/VulkanWrapper/Device.hpp"

#include <magic_enum.hpp>

#include <volk.h>

namespace Flashlight::VulkanWrapper {
    void Device::PickPhysicalDevice() {
        u32 deviceCount = 0;
        vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr);

        if (deviceCount == 0) {
            Log::EngineFatal({0x01, 0x01}, "Failed to find GPUs with Vulkan support.");
        }

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(m_Instance, &deviceCount, devices.data());

        std::multimap<int, VkPhysicalDevice> candidates;

        for (const auto& device : devices) {
            int score = RateDeviceSuitability(device);
            candidates.insert(std::make_pair(score, device));
        }

        if (candidates.rbegin()->first > 0) {
            m_PhysicalDevice = candidates.rbegin()->second;
        } else {
            Log::EngineFatal({0x01, 0x02}, "Failed to find a suitable GPU.");
        }

        m_PhysicalDeviceProperties = GetPhysicalDeviceProperties(m_PhysicalDevice);
        m_PhysicalDeviceFeatures = GetPhysicalDeviceFeatures(m_PhysicalDevice);
        m_QueueFamilies = FindQueueFamilies(m_PhysicalDevice);

        Log::EngineTrace("Physical device properties:");
        Log::EngineTrace("\t - Device name:           {0}", m_PhysicalDeviceProperties.deviceName);

        Log::EngineTrace("\t - Device type:           {0}",
                         magic_enum::enum_name(m_PhysicalDeviceProperties.deviceType));

        Log::EngineTrace("\t - Device API version:    {0}.{1}.{2}.{3}",
                         VK_API_VERSION_VARIANT(m_PhysicalDeviceProperties.apiVersion),
                         VK_API_VERSION_MAJOR(m_PhysicalDeviceProperties.apiVersion),
                         VK_API_VERSION_MINOR(m_PhysicalDeviceProperties.apiVersion),
                         VK_API_VERSION_PATCH(m_PhysicalDeviceProperties.apiVersion));

        // 4318 : NVIDIA
        if (m_PhysicalDeviceProperties.vendorID == 4318) {
            Log::EngineTrace("\t - Driver version:        {0}.{1}.{2}.{3}",
                             (m_PhysicalDeviceProperties.driverVersion >> 22) & 0x3FF,
                             (m_PhysicalDeviceProperties.driverVersion >> 14) & 0x0FF,
                             (m_PhysicalDeviceProperties.driverVersion >> 6) & 0x0FF,
                             (m_PhysicalDeviceProperties.driverVersion) & 0x003F);
        }

#if defined(_WIN32) || defined(_WIN64)
        // 0x8086 : Intel, only differs on Windows.
        else if (m_PhysicalDeviceProperties.vendorID == 0x8086) {
            Log::EngineTrace("\t Driver version:          {0}.{1}",
                             (m_PhysicalDeviceProperties.driverVersion >> 14),
                             (m_PhysicalDeviceProperties.driverVersion) & 0x3FFF);
        }
#endif
        // For other vendors, use the Vulkan version convention.
        else {
            Log::EngineTrace("\t Driver version:          {0}.{0}.{0}",
                             VK_API_VERSION_MAJOR(m_PhysicalDeviceProperties.driverVersion),
                             VK_API_VERSION_MINOR(m_PhysicalDeviceProperties.driverVersion),
                             VK_API_VERSION_PATCH(m_PhysicalDeviceProperties.driverVersion));
        }
    }

    void Device::CreateLogicalDevice(const DebugLevel& debugLevel) {
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<u32> uniqueQueueFamilies = {m_QueueFamilies.GraphicsFamily, m_QueueFamilies.PresentFamily};

        constexpr float queuePriority = 1.0f;
        for (u32 queueFamily : uniqueQueueFamilies) {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = m_QueueFamilies.GraphicsFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        // Keep the default features, we don't need more for now.
        constexpr VkPhysicalDeviceFeatures deviceFeatures{};

        VkDeviceCreateInfo deviceCreateInfo{};
        deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        deviceCreateInfo.queueCreateInfoCount = static_cast<u32>(queueCreateInfos.size());
        deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();

        deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

        // No extensions required for now.
        deviceCreateInfo.enabledExtensionCount = 0;
        deviceCreateInfo.ppEnabledExtensionNames = nullptr;

        if (debugLevel > DebugLevel::None) {
            deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(m_ValidationLayers.size());
            deviceCreateInfo.ppEnabledLayerNames = m_ValidationLayers.data();
        } else {
            deviceCreateInfo.enabledLayerCount = 0;
            deviceCreateInfo.ppEnabledLayerNames = nullptr;
        }

        if (vkCreateDevice(m_PhysicalDevice, &deviceCreateInfo, nullptr, &m_Device) != VK_SUCCESS) {
            Log::EngineFatal({0x01, 0x03}, "Failed to create logical device.");
        } else {
            Log::EngineTrace("Logical device created.");
        }

        volkLoadDevice(m_Device);

        vkGetDeviceQueue(m_Device, m_QueueFamilies.GraphicsFamily, 0, &m_GraphicsQueue);
        vkGetDeviceQueue(m_Device, m_QueueFamilies.PresentFamily, 0, &m_PresentQueue);
    }

    int Device::RateDeviceSuitability(const VkPhysicalDevice physicalDevice) const {
        if (!IsDeviceSuitable(physicalDevice)) {
            return 0;
        }

        int score = 0;

        const auto deviceProperties = GetPhysicalDeviceProperties(physicalDevice);

        if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            score += 1000;
        }

        score += static_cast<int>(deviceProperties.limits.maxImageDimension2D);

        return score;
    }

    bool Device::IsDeviceSuitable(const VkPhysicalDevice physicalDevice) const {
        const QueueFamilyIndices indices = FindQueueFamilies(physicalDevice);

        return indices.IsComplete();
    }

    VkPhysicalDeviceProperties Device::GetPhysicalDeviceProperties(const VkPhysicalDevice physicalDevice) {
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(physicalDevice, &properties);

        return properties;
    }

    VkPhysicalDeviceFeatures Device::GetPhysicalDeviceFeatures(const VkPhysicalDevice physicalDevice) {
        VkPhysicalDeviceFeatures features;
        vkGetPhysicalDeviceFeatures(physicalDevice, &features);

        return features;
    }

    QueueFamilyIndices Device::FindQueueFamilies(const VkPhysicalDevice physicalDevice) const {
        QueueFamilyIndices indices;

        u32 queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

        int i = 0;
        for (const auto& queueFamily : queueFamilies) {
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.GraphicsFamily = i;
                indices.FoundGraphicsFamily = true;
            }

            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, m_Surface, &presentSupport);

            if (presentSupport) {
                indices.PresentFamily = i;
                indices.FoundPresentFamily = true;
            }

            if (indices.IsComplete()) {
                break;
            }

            i++;
        }

        return indices;
    }
}
