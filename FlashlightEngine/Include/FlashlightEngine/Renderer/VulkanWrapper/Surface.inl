/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Surface.inl
 * Description : Definitions of inline methods from the Surface class.
 */
#pragma once

inline Surface::Surface(const Instance& instance, const Window& window) {
    m_Instance = instance.GetNativeInstance();
    m_Window = window.GetGlfwWindow();
    Create();
}

inline Surface::~Surface() {
    Destroy();
}

inline VkSurfaceKHR Surface::GetNativeSurface() const {
    return m_Surface;
}

inline void Surface::Destroy() const {
    if (m_Surface) {
        Log::EngineTrace("Destroying window surface.");
        vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
    }
}
