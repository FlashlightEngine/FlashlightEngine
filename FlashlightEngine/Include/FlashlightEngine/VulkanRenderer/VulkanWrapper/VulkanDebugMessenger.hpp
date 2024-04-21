#pragma once

#include "VulkanInstanceObject.hpp"

#include <vulkan/vulkan.h>

namespace Flashlight {

namespace VulkanWrapper {

    class VulkanInstance;

    class VulkanDebugMessenger
        : public VulkanInstanceObject<VulkanDebugMessenger, VkDebugUtilsMessengerEXT, VkDebugUtilsMessengerCreateInfoEXT> {
        VulkanDebugMessenger() = default;

        static inline void CreateHandle(VulkanInstance& instance, VkDebugUtilsMessengerCreateInfoEXT &createInfo,
                                        VkDebugUtilsMessengerEXT &debugMessenger);
        static inline void DestroyHandle(const VulkanInstance &instance, VkDebugUtilsMessengerEXT &debugMessenger);
    };

#include "VulkanDebugMessenger.inl"
    
}

}
