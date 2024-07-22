/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : DebugMessenger.hpp
 * Description : Declaration of the DebugMessenger class. The debug messenger is used to output debug messages
 *               from validation layers.
 */
#pragma once

#include "FlashlightEngine/Renderer/VulkanWrapper/Instance.hpp"

#include "FlashlightEngine/Core/Logger.hpp"

#include <volk.h>

namespace Flashlight::VulkanWrapper {
    class DebugMessenger {
        VkDebugUtilsMessengerEXT m_DebugMessenger = VK_NULL_HANDLE;
        VkInstance m_Instance = VK_NULL_HANDLE;
        
    public:
        inline DebugMessenger(const Instance& instance, const DebugLevel& debugLevel);
        inline ~DebugMessenger();

        DebugMessenger(const DebugMessenger&) = delete;
        DebugMessenger(DebugMessenger&&) = delete;

        DebugMessenger& operator=(const DebugMessenger&) = delete;
        DebugMessenger& operator=(DebugMessenger&&) = delete;

        [[nodiscard]] inline VkDebugUtilsMessengerEXT GetNativeDebugMessenger() const;

    private:
        void Create(const DebugLevel &debugLevel);
        inline void Destroy() const;
    };

#include "DebugMessenger.inl"
}
