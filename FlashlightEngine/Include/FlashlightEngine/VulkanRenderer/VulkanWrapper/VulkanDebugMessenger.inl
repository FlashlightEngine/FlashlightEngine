#pragma once

inline void VulkanDebugMessenger::CreateHandle(VulkanInstance& instance, VkDebugUtilsMessengerCreateInfoEXT &createInfo,
                                        VkDebugUtilsMessengerEXT &debugMessenger) {
        
    instance.PopulateDebugMessengerCreateInfo(createInfo);

    instance.vkCreateDebugUtilsMessengerEXT(instance.GetHandle(), &createInfo, nullptr, &debugMessenger);
}

inline void VulkanDebugMessenger::DestroyHandle(const VulkanInstance &instance, VkDebugUtilsMessengerEXT &debugMessenger) {
    instance.vkDestroyDebugUtilsMessengerEXT(instance.GetHandle(), debugMessenger, nullptr);
}