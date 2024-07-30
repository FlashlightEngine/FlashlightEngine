/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Instance.hpp
 * Description : Declaration of the Instance class. It is used to create a Vulkan instance and debug messenger.
 */
#pragma once

#include <FlashlightEngine/VulkanRenderer/VulkanTypes.hpp>

#include <FlashlightEngine/Core/Window.hpp>

#include <VkBootstrap.h>

namespace Flashlight::VulkanRenderer::VulkanWrapper {

    class Instance {
        VkInstance m_Instance = VK_NULL_HANDLE;                     // Vulkan library handle.
        VkDebugUtilsMessengerEXT m_DebugMessenger = VK_NULL_HANDLE; // Vulkan debug output handle.
        vkb::Instance m_VkbInstance;
        
    public:
        Instance(const Window& window, const DebugLevel& debugLevel);
        ~Instance();

        Instance(const Instance&) = delete;
        Instance(Instance&& other) noexcept;

        Instance& operator=(const Instance&) = delete;
        Instance& operator=(Instance&& other) noexcept;

        [[nodiscard]] inline VkInstance GetInstance() const;
        [[nodiscard]] inline vkb::Instance GetVkbInstance() const;
    };

#include "Instance.inl"
}
