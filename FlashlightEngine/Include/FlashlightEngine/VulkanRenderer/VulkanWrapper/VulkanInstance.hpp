#pragma once

#include "FlashlightEngine/pch.hpp"

#include <vulkan/vulkan.h>

namespace Flashlight {

namespace VulkanWrapper {

    class VulkanInstance {
    public:
        inline VulkanInstance();
        VulkanInstance(const VulkanInstance &) = delete;
        VulkanInstance(VulkanInstance &&) = delete;
        inline ~VulkanInstance();

        void Create(VkApplicationInfo appInfo, VkInstanceCreateInfo createInfo);

#if defined(FL_DEBUG)
        inline std::vector<const char*> GetEnabledValidationLayers() const noexcept;
#endif
        
        inline bool IsValid() const noexcept;
        inline VkInstance GetHandle() const noexcept;
        
        void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo) noexcept;

#define VK_INSTANCE_FUNCTION( fun ) static extern PFN_##fun fun;
#include "InstanceFunctions.hpp"
        
    private:
        VkInstance m_Handle;
        
        void Cleanup() const;

        // Helper functions.
        bool CheckValidationLayerSupport() const noexcept;
        std::vector<const char*> GetRequiredInstanceExtensions() const noexcept;
        void CheckRequiredInstanceExtensionsSupport() const;
        std::vector<VkExtensionProperties> GetAvailableInstanceExtensions() const noexcept;
        void LoadInstanceFunctions();

#if defined(FL_DEBUG)
        const std::vector<const char*> m_ValidationLayers = {
            "VK_LAYER_KHRONOS_validation"
        };
#endif
    };

#include "VulkanInstance.inl"
    
}

}
