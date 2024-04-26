#pragma once

#include "VulkanInstance.hpp"
#include "VulkanWindowSurface.hpp"

#include <vulkan/vulkan.h>

namespace Flashlight {

namespace VulkanWrapper {
    
    /// @ingroup VulkanRenderer
    /// @struct Flashlight::VulkanWrapper::SwapChainSupportDetails
    /// @brief Struct that stores properties for the swap chain.
    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR Capabilities;
        std::vector<VkSurfaceFormatKHR> Formats;
        std::vector<VkPresentModeKHR> PresentModes;
    };    

    /// @ingroup VulkanRenderer
    /// @struct Flashlight::VulkanWrapper::QueueFamilyIndices
    /// @brief Structure that stores queue family indicies and a utility function to check if they are both present.
    struct QueueFamilyIndices {
        u32 GraphicsFamily = 0;
        u32 PresentFamily = 0;
        bool GraphicsFamilyHasValue = false;
        bool PresentFamilyHasValue = false;

        /// @brief Function to check if every queue family is set.
        ///
        /// @returns A boolean telling if every queue family has a value.
        bool IsComplete() {
            return GraphicsFamilyHasValue && PresentFamilyHasValue;
        }
    };
    
    class VulkanDevice {
    public:
        inline VulkanDevice();
        VulkanDevice(const VulkanDevice &) = delete;
        VulkanDevice(const VulkanDevice &&) = delete;
        inline ~VulkanDevice();

        void Create(VulkanInstance &instance, const VulkanWindowSurface &surface);
        inline void Destroy() const;

        inline bool IsValid() const noexcept;
        inline VkDevice GetHandle() const noexcept;
        inline QueueFamilyIndices FindQueueFamilies() const noexcept;
        inline SwapChainSupportDetails QuerySwapChainSupport() const noexcept;
        inline VkPhysicalDeviceProperties GetDeviceProperties() const noexcept;
        inline VkPhysicalDeviceFeatures GetDeviceFeatures() const noexcept;
        
    private:

        VkPhysicalDevice m_PhysicalDevice{VK_NULL_HANDLE};
        VkPhysicalDeviceProperties m_DeviceProperties {};
        VkPhysicalDeviceFeatures m_DeviceFeatures {};
        VkDevice m_Device {};
        
        VkInstance m_Instance {};
        VkSurfaceKHR m_Surface {};

        void PickPhysicalDevice();
        
        void CreateLogicalDevice(VulkanInstance &instance);
        void DestroyLogicalDevice() const;
        
        inline VkPhysicalDeviceProperties GetDeviceProperties(VkPhysicalDevice physicalDevice) const noexcept;
        inline VkPhysicalDeviceFeatures GetDeviceFeatures(VkPhysicalDevice physicalDevice) const noexcept;
        bool IsDeviceSuitable(VkPhysicalDevice physicalDevice) const noexcept;
        bool CheckDeviceExtensionSupport(VkPhysicalDevice physicalDevice) const noexcept;
        inline std::vector<VkExtensionProperties> GetAvailableDeviceExtensions(VkPhysicalDevice physicalDevice) const noexcept;
        int RateDeviceSuitability(VkPhysicalDevice physicalDevice) const noexcept;
        QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice physicalDevice) const noexcept;
        SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice physicalDevice) const noexcept;

        const std::vector<const char*> m_DeviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };
    };

#include "VulkanDevice.inl"
    
}

}
