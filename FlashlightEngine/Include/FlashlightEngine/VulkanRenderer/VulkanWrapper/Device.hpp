/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Device.hpp
 * Description : Declaration of the Device class. It is used to select a physical device and create a device.
 */
#pragma once

#include <FlashlightEngine/VulkanRenderer/VulkanTypes.hpp>

#include <FlashlightEngine/VulkanRenderer/VulkanWrapper/Instance.hpp>
#include <FlashlightEngine/VulkanRenderer/VulkanWrapper/Surface.hpp>

namespace Flashlight::VulkanRenderer::VulkanWrapper {
    class Device {
        VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;         // GPU chosen as the default device.
        VkDevice m_Device = VK_NULL_HANDLE;                         // Vulkan device for commands.

        VkQueue m_GraphicsQueue = VK_NULL_HANDLE;
        u32 m_GraphicsQueueFamily = 0;
        VkQueue m_PresentQueue = VK_NULL_HANDLE;
        u32 m_PresentQueueFamily = 0;
        
        std::shared_ptr<Instance> m_Instance;
        
    public:
        Device(const std::shared_ptr<Instance>& instance, const Surface& surface);
        ~Device();

        Device(const Device&) = delete;
        Device(Device&& other) noexcept;

        Device& operator=(const Device&) = delete;
        Device& operator=(Device&& other) noexcept;

        [[nodiscard]] inline VkPhysicalDevice GetPhysicalDevice() const;
        [[nodiscard]] inline VkDevice GetLogicalDevice() const;
        [[nodiscard]] inline VkQueue GetGraphicsQueue() const;
        [[nodiscard]] inline u32 GetGraphicsQueueFamilyIndex() const;
        [[nodiscard]] inline VkQueue GetPresentQueue() const;
        [[nodiscard]] inline u32 GetPresentQueueFamilyIndex() const;
    };

#include "Device.inl"
}
