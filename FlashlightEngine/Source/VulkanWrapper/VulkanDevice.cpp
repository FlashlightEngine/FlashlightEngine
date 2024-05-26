#include "FlashlightEngine/VulkanWrapper/VulkanDevice.hpp"

#include "FlashlightEngine/Core/Log.hpp"

namespace Flashlight {

namespace VulkanWrapper {

    /// @brief Creates the Vulkan logical device.
    void VulkanDevice::Create(const VulkanInstance &instance, const VulkanPhysicalDevice &physicalDevice) {
        m_Instance = instance.GetInstance();

        QueueFamilyIndices indices = physicalDevice.FindQueueFamilies();

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

        auto requiredExtensions= physicalDevice.GetRequiredExtensions();

        createInfo.enabledExtensionCount = static_cast<u32>(requiredExtensions.size());
        createInfo.ppEnabledExtensionNames = requiredExtensions.data();

        const auto availableDeviceExtensions = physicalDevice.GetAvailableExtensions();
        FL_LOG("Available device extensions : ")
        for (const auto &extension : availableDeviceExtensions) {
            std::cout << '\t' << extension.extensionName << '\n';
        }

        FL_LOG("Require device extensions : ")
        for (const auto &extension : requiredExtensions) {
            std::cout << '\t' << extension << '\n';
        }
        
#if FL_DEBUG
        auto enabledValidationLayers = instance.GetEnabledValidationLayers();
        
        createInfo.enabledLayerCount = static_cast<u32>(enabledValidationLayers.size());
        createInfo.ppEnabledLayerNames = enabledValidationLayers.data();
#else
        createInfo.enabledLayerCount = 0;
#endif

        if (vkCreateDevice(physicalDevice.GetPhysicalDevice(), &createInfo, nullptr, &m_Device) != VK_SUCCESS) {
            FL_THROW("Failed to create logical device")
        }

        volkLoadDevice(m_Device);
    }
    
}

}
