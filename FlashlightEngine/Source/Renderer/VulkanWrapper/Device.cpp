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
        vkEnumeratePhysicalDevices(m_Instance.GetNativeInstance(), &deviceCount, nullptr);

        if (deviceCount == 0) {
            Log::EngineFatal({0x01, 0x01}, "Failed to find GPUs with Vulkan support.");
        }

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(m_Instance.GetNativeInstance(), &deviceCount, devices.data());

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
        m_SwapChainSupport = QuerySwapChainSupport(m_PhysicalDevice);

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
            Log::EngineTrace("\t - Driver version:          {0}.{1}",
                             (m_PhysicalDeviceProperties.driverVersion >> 14),
                             (m_PhysicalDeviceProperties.driverVersion) & 0x3FFF);
        }
#endif
        // For other vendors, use the Vulkan version convention.
        else {
            Log::EngineTrace("\t - Driver version:          {0}.{0}.{0}",
                             VK_API_VERSION_MAJOR(m_PhysicalDeviceProperties.driverVersion),
                             VK_API_VERSION_MINOR(m_PhysicalDeviceProperties.driverVersion),
                             VK_API_VERSION_PATCH(m_PhysicalDeviceProperties.driverVersion));
        }
    }

    void Device::CreateLogicalDevice(const DebugLevel& debugLevel) {
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        const std::set<u32> uniqueQueueFamilies = {m_QueueFamilies.GraphicsFamily, m_QueueFamilies.PresentFamily};

        constexpr float queuePriority = 1.0f;
        for (const u32 queueFamily : uniqueQueueFamilies) {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
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

        deviceCreateInfo.enabledExtensionCount = static_cast<u32>(m_DeviceExtensions.size());
        deviceCreateInfo.ppEnabledExtensionNames = m_DeviceExtensions.data();

        if (debugLevel > DebugLevel::None) {
            deviceCreateInfo.enabledLayerCount = static_cast<u32>(m_ValidationLayers.size());
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

        const std::vector<VkExtensionProperties> availableExtensions = GetAvailableDeviceExtensions(
            m_PhysicalDevice);
        Log::EngineTrace("Available Vulkan device extensions:");
        for (const auto& extension : availableExtensions) {
            Log::EngineTrace("\t - {0}", extension.extensionName);
        }

        Log::EngineTrace("Required Vulkan device extensions:");
        for (const auto& extension : m_DeviceExtensions) {
            Log::EngineTrace("\t - {0}", extension);
        }

        vkGetDeviceQueue(m_Device, m_QueueFamilies.GraphicsFamily, 0, &m_GraphicsQueue);
        vkGetDeviceQueue(m_Device, m_QueueFamilies.PresentFamily, 0, &m_PresentQueue);
    }

#pragma region Physical Device Utility
    int Device::RateDeviceSuitability(const VkPhysicalDevice physicalDevice) {
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

    bool Device::IsDeviceSuitable(const VkPhysicalDevice physicalDevice) {
        const QueueFamilyIndices indices = FindQueueFamilies(physicalDevice);

        const bool extensionsSupported = CheckDeviceExtensionsSupport(physicalDevice);

        bool swapChainAdequate = false;
        if (extensionsSupported) {
            const SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(physicalDevice);
            swapChainAdequate = !swapChainSupport.Formats.empty() && !swapChainSupport.PresentModes.empty();
        }

        return indices.IsComplete() && extensionsSupported && swapChainAdequate;
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
            vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, m_Surface.GetNativeSurface(), &presentSupport);

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

    bool Device::CheckDeviceExtensionsSupport(const VkPhysicalDevice physicalDevice) {
        const std::vector<VkExtensionProperties> availableExtensions = GetAvailableDeviceExtensions(physicalDevice);

        std::set<std::string> requiredExtensions(m_DeviceExtensions.begin(), m_DeviceExtensions.end());

        for (const auto& extension : availableExtensions) {
            requiredExtensions.erase(extension.extensionName);
        }

        return requiredExtensions.empty();
    }

    std::vector<VkExtensionProperties> Device::GetAvailableDeviceExtensions(const VkPhysicalDevice physicalDevice) {
        u32 extensionCount = 0;
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableExtensions.data());

        return availableExtensions;
    }

    SwapChainSupportDetails Device::QuerySwapChainSupport(const VkPhysicalDevice physicalDevice) const {
        SwapChainSupportDetails details;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, m_Surface.GetNativeSurface(),
                                                  &details.Capabilities);

        u32 formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_Surface.GetNativeSurface(), &formatCount, nullptr);

        if (formatCount != 0) {
            details.Formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_Surface.GetNativeSurface(), &formatCount,
                                                 details.Formats.data());
        }

        u32 presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_Surface.GetNativeSurface(), &presentModeCount,
                                                  nullptr);

        if (presentModeCount != 0) {
            details.PresentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_Surface.GetNativeSurface(),
                                                      &presentModeCount, details.PresentModes.data());
        }

        return details;
    }
#pragma endregion
}
