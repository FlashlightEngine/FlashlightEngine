/* Copyright (C) 2024 Jean "Pixfri" Letessier (jean.letessier@protonmail.com)
 * This file is part of "FlashLight Engine"
 * For conditions of distribution and use, see copyright notice in FlashLightEngine.hpp
 *
 * File : VulkanBase.cpp
 * Description : This file contains the definitions of methods from the Flashlight::VulkanDevice class.
 */

#include "FlashlightEngine/VulkanRenderer/VulkanWrapper/VulkanBase.hpp"

#include <GLFW/glfw3.h>

#include "FlashlightEngine/pch.hpp"

namespace Flashlight {

namespace VulkanWrapper {
    
    /// @ingroup VulkanRenderer
    /// @class Flashlight::VulkanWrapper::VulkanBase
    /// @brief VulkanRenderer wrapper class for base Vulkan objects.

    /// @brief Initializes all of the Vulkan objects this class is a wrapper of.
    void VulkanBase::Init() {
        CreateSurface();
        PickPhysicalDevice();
        CreateLogicalDevice();
    }

    /// @brief Destroys all the Vulkan objects this class is a wrapper of.
    void VulkanBase::Cleanup() {
        DestroyLogicalDevice();
        DestroySurface();
    }

    /// @brief Creates the window surface.
    void VulkanBase::CreateSurface() {
        if (glfwCreateWindowSurface(m_Vulkan.Instance, m_Window.GetNativeWindow(), nullptr, &m_Vulkan.Surface) != VK_SUCCESS) {
            Log::EngineError("Failed to create window surface.");
        }
    }

    /// @brief Destroys the window surface.
    void VulkanBase::DestroySurface() const {
        vkDestroySurfaceKHR(m_Vulkan.Instance, m_Vulkan.Surface, nullptr);
    }

    /// @brief Picks a Vulkan physical device.
    void VulkanBase::PickPhysicalDevice() {
        u32 deviceCount = 0;
        vkEnumeratePhysicalDevices(m_Vulkan.Instance, &deviceCount, nullptr);

        if (deviceCount == 0) {
            Log::EngineError("Failed to find GPUs with Vulkan support.");
        }

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(m_Vulkan.Instance, &deviceCount, devices.data());

        std::multimap<i32, VkPhysicalDevice> candidates;

        for (const auto &device : devices) {
            i32 score = RateDeviceSuitability(device);
            candidates.insert(std::make_pair(score, device));
        }

        if (candidates.rbegin()->first > 0) {
            m_Vulkan.PhysicalDevice = candidates.rbegin()->second;
        } else {
            Log::EngineError("Failed to find a suitable GPU.");
        }

        // Stores the properties for the chosen physical device.
        DeviceProperties = GetDeviceProperties(m_Vulkan.PhysicalDevice);

        std::cout << "GPU Informations : " << '\n'
        << '\t' << "GPU Name : " << DeviceProperties.deviceName << '\n'
        << '\t' <<"GPU Driver Version : " << DeviceProperties.driverVersion << '\n'
        << '\t' << "GPU Vulkan API Version : " << DeviceProperties.apiVersion << '\n' << std::endl;
    }

    /// @brief Creates the Vulkan logical device.
    void VulkanBase::CreateLogicalDevice() {
        QueueFamilyIndices indices = FindQueueFamilies(m_Vulkan.PhysicalDevice);

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<u32> uniqueQueueFamilies = {indices.GraphicsFamily, indices.PresentFamily};

        float queuePriority = 1.0f;
        for (u32 queueFamily : uniqueQueueFamilies) {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        VkPhysicalDeviceFeatures deviceFeatures{};

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        createInfo.queueCreateInfoCount = static_cast<u32>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();

        createInfo.pEnabledFeatures = &deviceFeatures;

        createInfo.enabledExtensionCount = static_cast<u32>(m_DeviceExtensions.size());
        createInfo.ppEnabledExtensionNames = m_DeviceExtensions.data();

        const auto availableDeviceExtensions = GetAvailableDeviceExtensions(m_Vulkan.PhysicalDevice);
        std::cout << "Available device extensions : \n";
        for (const auto &extension : availableDeviceExtensions) {
            std::cout << '\t' << extension.extensionName << '\n';
        }

        std::cout << std::endl;

        std::cout << "Required device extensions : \n";
        for (const auto &extension : m_DeviceExtensions) {
            std::cout << '\t' << extension << '\n';
        }

        std::cout << std::endl;

#if FL_DEBUG
        createInfo.enabledLayerCount = static_cast<u32>(m_ValidationLayers.size());
        createInfo.ppEnabledLayerNames = m_ValidationLayers.data();
#else
        createInfo.enabledLayerCount = 0;
#endif

        if (vkCreateDevice(m_Vulkan.PhysicalDevice, &createInfo, nullptr, &m_Vulkan.LogicalDevice) != VK_SUCCESS) {
            Log::EngineError("Failed to create logical device.");
        }

        vkGetDeviceQueue(m_Vulkan.LogicalDevice, indices.GraphicsFamily, 0, &m_Vulkan.GraphicsQueue);
        vkGetDeviceQueue(m_Vulkan.LogicalDevice, indices.PresentFamily, 0, &m_Vulkan.PresentQueue);
    }

    /// @brief Destroys the Vulkan logical device.
    void VulkanBase::DestroyLogicalDevice() const {
        vkDestroyDevice(m_Vulkan.LogicalDevice, nullptr);
    }

    /// @brief Gets a physical device properties.
    ///
    /// @param physicalDevice The physical device to get the properties of.
    ///
    /// @returns The physical device properties.
    VkPhysicalDeviceProperties VulkanBase::GetDeviceProperties(VkPhysicalDevice physicalDevice) const noexcept {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);

        return deviceProperties;
    }

    /// @brief Gets a physical device features.
    ///
    /// @param physicalDevice The physical device to get the properties of.
    ///
    /// @returns The physical device features.
    VkPhysicalDeviceFeatures VulkanBase::GetDeviceFeatures(VkPhysicalDevice physicalDevice) const noexcept {
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);

        return deviceFeatures;
    }

    /// @brief Checks if the provided device suits the requirements.
    ///
    /// @param physicalDevice The physical device to check.
    ///
    /// @returns A boolean telling if the provided device suits the requirements.
    bool VulkanBase::IsDeviceSuitable(VkPhysicalDevice physicalDevice) const noexcept {
        auto deviceProperties = GetDeviceProperties(physicalDevice);
        auto deviceFeatures = GetDeviceFeatures(physicalDevice);

        QueueFamilyIndices indices = FindQueueFamilies(physicalDevice);

        bool extensionsSupported = CheckDeviceExtensionSupport(physicalDevice);

        bool swapChainAdequate = false;
        if (extensionsSupported) {
            SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(physicalDevice);
            swapChainAdequate = !swapChainSupport.Formats.empty() && !swapChainSupport.PresentModes.empty();
        }

        return indices.IsComplete() && extensionsSupported && swapChainAdequate;
    }

    /// @brief Checks if required device extensions are supported by the provided device.
    ///
    /// @param physicalDevice The physical device to check.
    ///
    /// @returns A boolean telling if required device extensions are available.
    bool VulkanBase::CheckDeviceExtensionSupport(VkPhysicalDevice physicalDevice) const noexcept {

        std::vector<VkExtensionProperties> availableExtensions = GetAvailableDeviceExtensions(physicalDevice);

        std::set<std::string> requiredExtensions(m_DeviceExtensions.begin(), m_DeviceExtensions.end());

        for (const auto &extension : availableExtensions) {
            requiredExtensions.erase(extension.extensionName);
        }

        return requiredExtensions.empty();
    }

    /// @brief Returns available extensions for the provided device.
    ///
    /// @param physicalDevice The physical device to check.
    ///
    /// @returns A std::vector of the extensions supported by the device.
    std::vector<VkExtensionProperties> VulkanBase::GetAvailableDeviceExtensions(VkPhysicalDevice physicalDevice) const noexcept {
        u32 extensionCount = 0;
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableExtensions.data());

        return availableExtensions;
    }

    /// @brief Rates devices with a score to help select it.
    ///
    /// @param physicalDevice The physical device to rate.
    ///
    /// @returns The score of the provided device.
    i32 VulkanBase::RateDeviceSuitability(VkPhysicalDevice physicalDevice) const noexcept {
        if (!IsDeviceSuitable(physicalDevice)) {
            // Devices that aren't suitable are scored 0.
            return 0;
        }

        i32 score = 0;

        auto deviceProperties = GetDeviceProperties(physicalDevice);

        // Discrete GPUs have a significant performance advantage.
        if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            score += 1000;
        }

        // Maximum possible size of textures affects graphics quality.
        score += static_cast<i32>(deviceProperties.limits.maxImageDimension2D);

        return score;
    }

    /// @brief Retrieve queue families from the physical device and returns them.
    ///
    /// @param physicalDevice The physical device to retrieve the queue families from.
    ///
    /// @returns A QueueFamilyIndices struct.
    QueueFamilyIndices VulkanBase::FindQueueFamilies(VkPhysicalDevice physicalDevice) const noexcept {
        QueueFamilyIndices indices;

        u32 queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

        i32 i = 0;
        for (const auto &queueFamily : queueFamilies) {
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.GraphicsFamily = i;
                indices.GraphicsFamilyHasValue = true;
            }

            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, m_Vulkan.Surface, &presentSupport);

            if (presentSupport) {
                indices.PresentFamily = i;
                indices.PresentFamilyHasValue = true;
            }

            if (indices.IsComplete()) {
                break;
            }

            i++;
        }

        return indices;
    }

    /// @brief Retrieves the swap chain support details for the provided device.
    ///
    /// @param physicalDevice The device to query the details from.
    ///
    /// @returns A SwapChainSupportDetails struct containing the details.
    SwapChainSupportDetails VulkanBase::QuerySwapChainSupport(VkPhysicalDevice physicalDevice) const noexcept {
        SwapChainSupportDetails details;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, m_Vulkan.Surface, &details.Capabilities);

        u32 formatCount = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_Vulkan.Surface, &formatCount, nullptr);

        if (formatCount != 0) {
            details.Formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_Vulkan.Surface, &formatCount, details.Formats.data());
        }

        u32 presentModeCount = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_Vulkan.Surface, &presentModeCount, nullptr);

        if (presentModeCount != 0) {
            details.PresentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_Vulkan.Surface, &presentModeCount, details.PresentModes.data());
        }

        return details;
    }
    
}

}