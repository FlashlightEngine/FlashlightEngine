/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Device.hpp
 * Description : Declaration of the Device class. It is used to chose a physical device and create a device and queues. 
 */
#pragma once

#include <FlashlightEngine/VulkanRenderer/VulkanTypes.hpp>
#include <FlashlightEngine/VulkanRenderer/VulkanWrapper/Instance.hpp>

namespace Flashlight::VulkanRenderer::VulkanWrapper {
    class Device {
        VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
        VkDevice m_Device = VK_NULL_HANDLE;
        vkb::Device m_VkbDevice;

        VkQueue m_GraphicsQueue = VK_NULL_HANDLE;
        u32 m_GraphicsQueueFamilyIndex = 0;

        VkQueue m_PresentQueue = VK_NULL_HANDLE;
        u32 m_PresentQueueFamilyIndex = 0;

        DeletionQueue m_DeletionQueue;

        bool m_Initialized = false;

    public:
        explicit Device(const Instance& instance);
        ~Device();

        Device(const Device&) = delete;
        Device(Device&&) = delete;

        Device& operator=(const Device&) = delete;
        Device& operator=(Device&&) = delete;

        [[nodiscard]] inline VkPhysicalDevice GetPhysicalDevice() const;
        [[nodiscard]] inline VkDevice GetDevice() const;
        [[nodiscard]] inline vkb::Device GetVkbDevice() const;
        [[nodiscard]] inline VkQueue GetGraphicsQueue() const;
        [[nodiscard]] inline u32 GetGraphicsQueueFamilyIndex() const;
        [[nodiscard]] inline VkQueue GetPresentQueue() const;
        [[nodiscard]] inline u32 GetPresentQueueFamilyIndex() const;
    };

#include <FlashlightEngine/VulkanRenderer/VulkanWrapper/Device.inl>
}
