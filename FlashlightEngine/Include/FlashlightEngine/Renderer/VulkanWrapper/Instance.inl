/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Instance.inl
 * Description : Definitions of inline methods from the Instance class.
 */
#pragma once

inline Instance::~Instance() {
    Destroy();
}

inline VkInstance Instance::GetNativeInstance() const {
    return m_Instance;
}

inline std::vector<const char*> Instance::GetValidationLayers() const {
    return m_ValidationLayers;
}

inline void Instance::Destroy() const {
    if (m_DebugMessenger) {
        Log::EngineTrace("Destroying Vulkan debug messenger.");
        vkDestroyDebugUtilsMessengerEXT(m_Instance, m_DebugMessenger, nullptr);
    }
    
    if (m_Instance) {
        Log::EngineTrace("Destroying Vulkan instance.");
        vkDestroyInstance(m_Instance, nullptr);
    }
}
