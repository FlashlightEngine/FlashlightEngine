#include "FlashlightEngine/VulkanRenderer/VulkanWrapper/VulkanDebugMessenger.hpp"

namespace Flashlight {

namespace VulkanWrapper {

    void VulkanDebugMessenger::Create(VulkanInstance &instance, VkDebugUtilsMessengerCreateInfoEXT &createInfo) {
        instance.PopulateDebugMessengerCreateInfo(createInfo);

        instance.vkCreateDebugUtilsMessengerEXT(instance.GetHandle(), &createInfo, nullptr, &m_Handle);
    }

    void VulkanDebugMessenger::DestroyHandle(const VulkanInstance &instance, VkDebugUtilsMessengerEXT debugMessenger) {
        instance.vkDestroyDebugUtilsMessengerEXT(instance.GetHandle(), debugMessenger, nullptr);
    }
    
}

}
