#pragma once

#include "VulkanInstance.hpp"
#include "VulkanWindowSurface.hpp"

#include <volk.h>

#include "FlashlightEngine/pch.hpp"

namespace Flashlight {

namespace VulkanWrapper {

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
        bool IsComplete() const noexcept {
            return GraphicsFamilyHasValue && PresentFamilyHasValue;
        }
    };

    /// @ingroup VulkanRenderer
    /// @struct Flashlight::VulkanWrapper::SwapChainSupportDetails
    /// @brief Struct that stores properties for the swap chain.
    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR Capabilities;
        std::vector<VkSurfaceFormatKHR> Formats;
        std::vector<VkPresentModeKHR> PresentModes;
    };

    class VulkanPhysicalDevice {
    public:
        inline VulkanPhysicalDevice();
        VulkanPhysicalDevice(const VulkanPhysicalDevice &) = delete;
        VulkanPhysicalDevice(VulkanPhysicalDevice &&) = delete;
        ~VulkanPhysicalDevice() = default;

        inline bool IsValid() const noexcept;
        inline VkPhysicalDevice GetPhysicalDevice() const noexcept;
        inline VkPhysicalDeviceProperties GetPhysicalDeviceProperties() const noexcept;
        inline VkPhysicalDeviceFeatures GetPhysicalDeviceFeatures() const noexcept;
        inline std::vector<VkExtensionProperties> GetAvailableExtensions() const noexcept;
        inline QueueFamilyIndices FindQueueFamilies() const noexcept;
        inline SwapChainSupportDetails GetSwapChainSupport() const noexcept;
        inline std::vector<const char*> GetRequiredExtensions() const noexcept;

        void PickPhysicalDevice(const VulkanInstance &instance, const VulkanWindowSurface &surface);

    private:
        VkPhysicalDevice m_PhysicalDevice;

        VkSurfaceKHR m_Surface;

        bool IsDeviceSuitable(VkPhysicalDevice physicalDevice) const noexcept;
        bool CheckDeviceExtensionSupport(VkPhysicalDevice physicalDevice) const noexcept;
        i32 RateDeviceSuitability(VkPhysicalDevice physicalDevice) const noexcept;

        inline VkPhysicalDeviceProperties GetPhysicalDeviceProperties(VkPhysicalDevice physicalDevice) const noexcept;
        inline VkPhysicalDeviceFeatures GetPhysicalDeviceFeatures(VkPhysicalDevice physicalDevice) const noexcept;
        inline std::vector<VkExtensionProperties> GetAvailableExtensions(VkPhysicalDevice physicalDevice) const noexcept;

        QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice physicalDevice) const noexcept;
        SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice physicalDevice) const noexcept;

        const std::vector<const char*> m_DeviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };
    };

#include "VulkanPhysicalDevice.inl"

}

}
