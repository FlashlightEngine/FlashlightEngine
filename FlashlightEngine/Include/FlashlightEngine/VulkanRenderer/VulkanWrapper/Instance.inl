/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Instance.inl
 * Description : Definitions of inline methods from the Instance class.
 */
#pragma once

inline VkInstance Instance::GetInstance() const {
    return m_Instance;
}

inline VkDebugUtilsMessengerEXT Instance::GetDebugMessenger() const {
    return m_DebugMessenger;
}

inline VkSurfaceKHR Instance::GetSurface() const {
    return m_Surface;
}

inline vkb::Instance Instance::GetVkbInstance() const {
    return m_VkbInstance;
}
