#include "FlashlightEngine/VulkanRenderer/VulkanWrapper/VulkanDevice.hpp"

namespace Flashlight {

namespace VulkanWrapper {

    void VulkanDevice::Create(VulkanInstance &instance, const VulkanWindowSurface &surface) {
        m_Instance = instance.GetHandle();
        m_Surface = surface.GetHandle();

        PickPhysicalDevice();
        CreateLogicalDevice(instance);
    }
    
    /// @brief Picks a Vulkan physical device.
    void VulkanDevice::PickPhysicalDevice() {
        u32 deviceCount = 0;
        vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr);

        if (deviceCount == 0) {
            Log::EngineError("Failed to find GPUs with Vulkan support.");
        }

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(m_Instance, &deviceCount, devices.data());

        std::multimap<i32, VkPhysicalDevice> candidates;

        for (const auto &device : devices) {
            i32 score = RateDeviceSuitability(device);
            candidates.insert(std::make_pair(score, device));
        }

        if (candidates.rbegin()->first > 0) {
            m_PhysicalDevice = candidates.rbegin()->second;
        } else {
            Log::EngineError("Failed to find a suitable GPU.");
        }

        // Stores the properties for the chosen physical device.
        m_DeviceProperties = GetDeviceProperties(m_PhysicalDevice);

        std::cout << std::format("GPU Information : \n GPU Name : {} \n GPU Driver Version : {} \n GPU Vulkan API Version : {}\n",
            m_DeviceProperties.deviceName, m_DeviceProperties.driverVersion, m_DeviceProperties.apiVersion);
    }
    
    /// @brief Creates the Vulkan logical device.
    void VulkanDevice::CreateLogicalDevice(VulkanInstance &instance) {
        QueueFamilyIndices indices = FindQueueFamilies(m_PhysicalDevice);

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

        VkPhysicalDeviceFeatures deviceFeatures {};

        VkDeviceCreateInfo createInfo {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        createInfo.queueCreateInfoCount = static_cast<u32>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();

        createInfo.pEnabledFeatures = &deviceFeatures;

        createInfo.enabledExtensionCount = static_cast<u32>(m_DeviceExtensions.size());
        createInfo.ppEnabledExtensionNames = m_DeviceExtensions.data();

        const auto availableDeviceExtensions = GetAvailableDeviceExtensions(m_PhysicalDevice);
        std::cout << "Available device extensions :\n";
        for (const auto &extension : availableDeviceExtensions) {
            std::cout << std::format("\t {} \n", extension.extensionName);
        }

        Log::EngineInfo("Required device extensions :");
        for (const auto &extension : m_DeviceExtensions) {
            std::cout << std::format("\t {} \n", extension);
        }
        
#if FL_DEBUG
        auto enabledValidationLayers = instance.GetEnabledValidationLayers();
        
        createInfo.enabledLayerCount = static_cast<u32>(enabledValidationLayers.size());
        createInfo.ppEnabledLayerNames = enabledValidationLayers.data();
#else
        createInfo.enabledLayerCount = 0;
#endif

        if (vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_Device) != VK_SUCCESS) {
            std::cout << "Failed to create logical device.";
        }

        volkLoadDevice(m_Device);
    }

    /// @brief Destroys the Vulkan logical device.
    void VulkanDevice::DestroyLogicalDevice() const {
        if (IsValid()) {
            vkDestroyDevice(m_Device, nullptr);
        }
    }
    
    /// @brief Checks if the provided device suits the requirements.
    ///
    /// @param physicalDevice The physical device to check.
    ///
    /// @returns A boolean telling if the provided device suits the requirements.
    bool VulkanDevice::IsDeviceSuitable(VkPhysicalDevice physicalDevice) const noexcept {
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
    bool VulkanDevice::CheckDeviceExtensionSupport(VkPhysicalDevice physicalDevice) const noexcept {

        std::vector<VkExtensionProperties> availableExtensions = GetAvailableDeviceExtensions(physicalDevice);

        std::set<std::string> requiredExtensions(m_DeviceExtensions.begin(), m_DeviceExtensions.end());

        for (const auto &extension : availableExtensions) {
            requiredExtensions.erase(extension.extensionName);
        }

        return requiredExtensions.empty();
    }

    /// @brief Rates devices with a score to help select it.
    ///
    /// @param physicalDevice The physical device to rate.
    ///
    /// @returns The score of the provided device.
    i32 VulkanDevice::RateDeviceSuitability(VkPhysicalDevice physicalDevice) const noexcept {
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
    QueueFamilyIndices VulkanDevice::FindQueueFamilies(VkPhysicalDevice physicalDevice) const noexcept {
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
            vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, m_Surface, &presentSupport);

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
    SwapChainSupportDetails VulkanDevice::QuerySwapChainSupport(VkPhysicalDevice physicalDevice) const noexcept {
        SwapChainSupportDetails details;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, m_Surface, &details.Capabilities);

        u32 formatCount = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_Surface, &formatCount, nullptr);

        if (formatCount != 0) {
            details.Formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_Surface, &formatCount, details.Formats.data());
        }

        u32 presentModeCount = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_Surface, &presentModeCount, nullptr);

        if (presentModeCount != 0) {
            details.PresentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_Surface, &presentModeCount, details.PresentModes.data());
        }

        return details;
    }
    
}

}
