#pragma once

#include "FlashlightEngine/pch.hpp"

#include <volk.h>

namespace Flashlight {

namespace VulkanWrapper {

    class VulkanInstance {
    public:
        inline VulkanInstance();
        VulkanInstance(const VulkanInstance &) = delete;
        VulkanInstance(VulkanInstance &&) = delete;
        inline ~VulkanInstance();

        void Create();

        inline std::vector<const char*> GetEnabledValidationLayers() const noexcept;
        
        inline bool IsValid() const noexcept;
        inline VkInstance GetInstance() const noexcept;
        
        void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo) noexcept;
        
    private:
        VkInstance m_Handle;
        
        void Cleanup() const;

        // Helper functions.
        bool CheckValidationLayerSupport() const noexcept;
        std::vector<const char*> GetRequiredInstanceExtensions() const noexcept;
        void CheckRequiredInstanceExtensionsSupport() const;
        std::vector<VkExtensionProperties> GetAvailableInstanceExtensions() const noexcept;

        const std::vector<const char*> m_ValidationLayers = {
            "VK_LAYER_KHRONOS_validation"
        };
    };

#include "VulkanInstance.inl"
    
}

}
