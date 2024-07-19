/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : SurfaceConfigurator.inl
 * Description : Definitions of inline methods from the SurfaceConfigurator class.
 */
#pragma once

inline void SurfaceConfigurator::Unconfigure(WGPUSurface surface) {
    wgpuSurfaceUnconfigure(surface);
}
