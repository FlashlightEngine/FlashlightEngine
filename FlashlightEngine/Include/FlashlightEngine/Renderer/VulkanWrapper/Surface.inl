/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Surface.inl
 * Description : Definitions of inline methods from the Surface class.
 */
#pragma once

inline Surface::~Surface() {
    if (m_Surface) {
        Log::EngineTrace("Destroying window surface.");
        vkDestroySurfaceKHR(m_Instance.GetNativeInstance(), m_Surface, nullptr);
    }
}

inline VkSurfaceKHR Surface::GetNativeSurface() const {
    return m_Surface;
}