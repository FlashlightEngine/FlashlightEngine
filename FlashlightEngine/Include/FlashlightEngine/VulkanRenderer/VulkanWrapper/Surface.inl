/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Surface.inl
 * Description : Definitions of methods from the Surface class.
 */
#pragma once

inline VkSurfaceKHR Surface::GetSurface() const {
    return m_Surface;
}
