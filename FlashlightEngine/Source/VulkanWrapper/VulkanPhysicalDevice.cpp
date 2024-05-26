#include "FlashlightEngine/VulkanWrapper/VulkanPhysicalDevice.hpp"

#include "FlashlightEngine/Core/Log.hpp"

namespace Flashlight {

namespace VulkanWrapper {

    /// @brief Picks a Vulkan physical device.
    void VulkanPhysicalDevice::PickPhysicalDevice(const VulkanInstance &instance, const VulkanWindowSurface &surface) {
        m_Surface = surface.GetSurface();

        u32 deviceCount = 0;
        vkEnumeratePhysicalDevices(instance.GetInstance(), &deviceCount, nullptr);

        if (deviceCount == 0) {
            FL_THROW("Failed to find a GPU with Vulkan support.")
        }

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(instance.GetInstance(), &deviceCount, devices.data());

        std::multimap<i32, VkPhysicalDevice> candidates;

        for (const auto &device : devices) {
            i32 score = RateDeviceSuitability(device);
            candidates.insert(std::make_pair(score, device));
        }

        if (candidates.rbegin()->first > 0) {
            m_PhysicalDevice = candidates.rbegin()->second;
        } else {
            FL_THROW("Failed to find a suitable GPU.")
        }
    }

    /// @brief Checks if the provided device suits the requirements.
    ///
    /// @param physicalDevice The physical device to check.
    ///
    /// @returns A boolean telling if the provided device suits the requirements.
    bool VulkanPhysicalDevice::IsDeviceSuitable(VkPhysicalDevice physicalDevice) const noexcept {
        auto deviceProperties = GetPhysicalDeviceProperties(physicalDevice);
        auto deviceFeatures = GetPhysicalDeviceFeatures(physicalDevice);

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
    bool VulkanPhysicalDevice::CheckDeviceExtensionSupport(VkPhysicalDevice physicalDevice) const noexcept {

        std::vector<VkExtensionProperties> availableExtensions = GetAvailableExtensions(physicalDevice);

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
    i32 VulkanPhysicalDevice::RateDeviceSuitability(VkPhysicalDevice physicalDevice) const noexcept {
        if (!IsDeviceSuitable(physicalDevice)) {
            // Devices that aren't suitable are scored 0.
            return 0;
        }

        i32 score = 0;

        auto deviceProperties = GetPhysicalDeviceProperties(physicalDevice);

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
    QueueFamilyIndices VulkanPhysicalDevice::FindQueueFamilies(VkPhysicalDevice physicalDevice) const noexcept {
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
    SwapChainSupportDetails VulkanPhysicalDevice::QuerySwapChainSupport(VkPhysicalDevice physicalDevice) const noexcept {
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