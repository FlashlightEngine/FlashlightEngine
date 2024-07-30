/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Device.cpp
 * Description : Definitions of methods from the Device class.
 */
#include <FlashlightEngine/VulkanRenderer/VulkanWrapper/Device.hpp>

#include <magic_enum.hpp>

namespace Flashlight::VulkanRenderer::VulkanWrapper {
    Device::Device(const std::shared_ptr<Instance>& instance, const Surface& surface) : m_Instance(instance) {
        // Vulkan 1.3 features
        VkPhysicalDeviceVulkan13Features features = {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES
        };
        features.dynamicRendering = VK_TRUE;
        features.synchronization2 = VK_TRUE;

        // Vulkan 1.2 features
        VkPhysicalDeviceVulkan12Features features12 = {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES
        };
        features12.bufferDeviceAddress = VK_TRUE;
        features12.descriptorIndexing = VK_TRUE;

        Log::EngineTrace("Selecting Vulkan physical device & creating Vulkan logical device...");
        vkb::PhysicalDeviceSelector selector{instance->GetVkbInstance()};
        vkb::PhysicalDevice physicalDevice = selector
                                             .set_minimum_version(1, 3)
                                             .set_required_features_13(features)
                                             .set_required_features_12(features12)
                                             .set_surface(surface.GetSurface())
                                             .select()
                                             .value();

        // Create the final Vulkan device.
        vkb::DeviceBuilder deviceBuilder{physicalDevice};

        vkb::Device vkbDevice = deviceBuilder.build().value();

        Log::EngineTrace("Vulkan physical device selected & logical device created.");
        m_Device = vkbDevice.device;
        m_PhysicalDevice = physicalDevice.physical_device;

        volkLoadDevice(m_Device);

        VkPhysicalDeviceProperties physicalDeviceProperties;
        vkGetPhysicalDeviceProperties(m_PhysicalDevice, &physicalDeviceProperties);

        Log::EngineTrace("Vulkan physical device properties:");
        Log::EngineTrace("\t - Device name:           {0}", physicalDeviceProperties.deviceName);

        Log::EngineTrace("\t - Device type:           {0}",
                         magic_enum::enum_name(physicalDeviceProperties.deviceType));

        Log::EngineTrace("\t - Device API version:    {0}.{1}.{2}.{3}",
                         VK_API_VERSION_VARIANT(physicalDeviceProperties.apiVersion),
                         VK_API_VERSION_MAJOR(physicalDeviceProperties.apiVersion),
                         VK_API_VERSION_MINOR(physicalDeviceProperties.apiVersion),
                         VK_API_VERSION_PATCH(physicalDeviceProperties.apiVersion));

        // 4318 : NVIDIA
        if (physicalDeviceProperties.vendorID == 4318) {
            Log::EngineTrace("\t - Driver version:        {0}.{1}.{2}.{3}",
                             (physicalDeviceProperties.driverVersion >> 22) & 0x3FF,
                             (physicalDeviceProperties.driverVersion >> 14) & 0x0FF,
                             (physicalDeviceProperties.driverVersion >> 6) & 0x0FF,
                             (physicalDeviceProperties.driverVersion) & 0x003F);
        }

#if defined(_WIN32) || defined(_WIN64)
        // 0x8086 : Intel, only differs on Windows.
        else if (physicalDeviceProperties.vendorID == 0x8086) {
            Log::EngineTrace("\t - Driver version:          {0}.{1}",
                             (physicalDeviceProperties.driverVersion >> 14),
                             (physicalDeviceProperties.driverVersion) & 0x3FFF);
        }
#endif
        // For other vendors, use the Vulkan version convention.
        else {
            Log::EngineTrace("\t - Driver version:          {0}.{0}.{0}",
                             VK_API_VERSION_MAJOR(physicalDeviceProperties.driverVersion),
                             VK_API_VERSION_MINOR(physicalDeviceProperties.driverVersion),
                             VK_API_VERSION_PATCH(physicalDeviceProperties.driverVersion));
        }

        m_GraphicsQueue = vkbDevice.get_queue(vkb::QueueType::graphics).value();
        m_GraphicsQueueFamily = vkbDevice.get_queue_index(vkb::QueueType::graphics).value();
        
    }

    Device::~Device() {
        if (m_Device) {
            Log::EngineTrace("Destroying Vulkan device.");
            vkDestroyDevice(m_Device, nullptr);
        }
    }

    Device::Device(Device&& other) noexcept {
        m_PhysicalDevice = other.m_PhysicalDevice;
        m_Device = other.m_Device;
        m_GraphicsQueue = other.m_GraphicsQueue;
        m_GraphicsQueueFamily = other.m_GraphicsQueueFamily;

        m_Instance = std::move(other.m_Instance);
    }

    Device& Device::operator=(Device&& other) noexcept {
        m_PhysicalDevice = other.m_PhysicalDevice;
        m_Device = other.m_Device;
        m_GraphicsQueue = other.m_GraphicsQueue;
        m_GraphicsQueueFamily = other.m_GraphicsQueueFamily;

        m_Instance = std::move(other.m_Instance);

        return *this;
    }
}
