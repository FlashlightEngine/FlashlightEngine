/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Instance.hpp
 * Description : Declaration of the Instance class. An instance within Vulkan is an object used to store per-application
 *               states. It therefor stores per-application information and is used to create other per-application
 *               objects.
 */
#pragma once

#include "FlashlightEngine/pch.hpp"

#include <volk.h>

namespace Flashlight::VulkanWrapper {
    class Instance {
        VkInstance m_Instance = VK_NULL_HANDLE;

    public:
        inline Instance();
        inline ~Instance();

        Instance(const Instance&) = delete;
        Instance(Instance&&) = delete;

        Instance& operator=(const Instance&) = delete;
        Instance& operator=(Instance&&) = delete;

        [[nodiscard]] inline VkInstance GetNativeInstance() const;

    private:
        void Create();
        inline void Destroy() const;

        static std::vector<const char*> GetRequiredInstanceExtensions();
        static void CheckRequiredInstanceExtensionsSupport();
        static std::vector<VkExtensionProperties> GetAvailableInstanceExtensions();
    };

#include "Instance.inl"
}
