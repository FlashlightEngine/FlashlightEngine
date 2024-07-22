/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : DebugMessenger.inl
 * Description : Definitions of inline methods from the DebugMessenger class.
 */
#pragma once

inline DebugMessenger::DebugMessenger(const Instance& instance, const DebugLevel& debugLevel) {
    m_Instance = instance.GetNativeInstance();
    Create(debugLevel);
}

inline DebugMessenger::~DebugMessenger() {
    Destroy();
}

inline VkDebugUtilsMessengerEXT DebugMessenger::GetNativeDebugMessenger() const {
    return m_DebugMessenger;
}

inline void DebugMessenger::Destroy() const {
    if (m_DebugMessenger) {
        Log::EngineTrace("Destroying Vulkan debug messenger.");
        vkDestroyDebugUtilsMessengerEXT(m_Instance, m_DebugMessenger, nullptr);
    }
}
