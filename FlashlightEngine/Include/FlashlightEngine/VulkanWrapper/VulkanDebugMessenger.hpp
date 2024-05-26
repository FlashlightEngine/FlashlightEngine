#pragma once

#include "VulkanInstance.hpp"

#include <volk.h>

namespace Flashlight {

namespace VulkanWrapper {

    class VulkanDebugMessenger {
    public:
        inline VulkanDebugMessenger();
        VulkanDebugMessenger(const VulkanDebugMessenger &) = delete;
        VulkanDebugMessenger(VulkanDebugMessenger &&) = delete;
        inline ~VulkanDebugMessenger();

        inline bool IsValid() const noexcept;
        inline VkDebugUtilsMessengerEXT GetHandle() const noexcept;
        
        void Create(VulkanInstance& instance);
    private:
        VkDebugUtilsMessengerEXT m_Handle;
        VkInstance m_Instance;
        
        void Destroy() const;
    };

#include "VulkanDebugMessenger.inl"
    
}

}
