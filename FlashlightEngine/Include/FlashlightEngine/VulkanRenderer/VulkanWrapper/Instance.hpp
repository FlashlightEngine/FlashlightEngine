/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Instance.hpp
 * Description : Declaration of the Instance class. It is used to create and destroy the Vulkan instance, debug
 *               messenger and surface.
 */
#pragma once

#include <FlashlightEngine/VulkanRenderer/VulkanTypes.hpp>

#include <FlashlightEngine/Core/Window.hpp>

#include <VkBootstrap.h>

namespace Flashlight::Renderer::VulkanWrapper {
    class Instance {
        vkb::Instance m_VkbInstance;
        VkInstance m_Instance = VK_NULL_HANDLE;
        VkDebugUtilsMessengerEXT m_DebugMessenger = VK_NULL_HANDLE;
        VkSurfaceKHR m_Surface = VK_NULL_HANDLE;

        bool m_Initialized = false;

        DeletionQueue m_DeletionQueue;

    public:
        Instance(const Window &window, const DebugLevel& debugLevel);
        ~Instance();

        Instance(const Instance&) = delete;
        Instance(Instance&&) = delete;

        Instance& operator=(const Instance&) = delete;
        Instance& operator=(Instance&&) = delete;

        [[nodiscard]] inline VkInstance GetInstance() const;
        [[nodiscard]] inline VkDebugUtilsMessengerEXT GetDebugMessenger() const;
        [[nodiscard]] inline VkSurfaceKHR GetSurface() const;
        [[nodiscard]] inline vkb::Instance GetVkbInstance() const;
    };

#include <FlashlightEngine/VulkanRenderer/VulkanWrapper/Instance.inl>
}
