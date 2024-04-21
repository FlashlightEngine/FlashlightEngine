#pragma once

#include "VulkanInstanceObject.hpp"

#include <vulkan/vulkan.h>

namespace Flashlight {

namespace VulkanWrapper {

    class VulkanInstance;

    class VulkanDebugMessenger
        : public VulkanInstanceObject<VulkanDebugMessenger, VkDebugUtilsMessengerEXT, VkDebugUtilsMessengerCreateInfoEXT> {
        VulkanDebugMessenger() = default;

        void Create(VulkanInstance &instance, VkDebugUtilsMessengerCreateInfoEXT &createInfo) override;

        static void DestroyHandle(const VulkanInstance &instance, VkDebugUtilsMessengerEXT debugMessenger);
    };

#include "VulkanDebugMessenger.inl"
    
}

}
