#pragma once

#include "VulkanInstance.hpp"
#include "VulkanPhysicalDevice.hpp"

#include <vulkan/vulkan.h>

namespace Flashlight {

namespace VulkanWrapper {
    class VulkanDevice {
    public:
        inline VulkanDevice();
        VulkanDevice(const VulkanDevice &) = delete;
        VulkanDevice(const VulkanDevice &&) = delete;
        inline ~VulkanDevice();

        void Create(const VulkanInstance &instance, const VulkanPhysicalDevice &physicalDevice);

        inline bool IsValid() const noexcept;
        inline VkDevice GetDevice() const noexcept;
        
    private:
        VkDevice m_Device {};
        
        VkInstance m_Instance {};

        inline void Destroy() const;
    };

#include "VulkanDevice.inl"
    
}

}
