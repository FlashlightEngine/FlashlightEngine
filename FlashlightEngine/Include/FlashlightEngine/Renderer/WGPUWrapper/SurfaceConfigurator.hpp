/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : SurfaceConfigurator.hpp
 * Description : Declaration of the SurfaceConfigurator class. It is used to configure the surface.
 */
#pragma once

#include "FlashlightEngine/pch.hpp"

namespace Flashlight::WGPUWrapper {
    struct SurfaceConfiguration {
        u32 Width, Height;
    };

    class SurfaceConfigurator {
    public:
        SurfaceConfigurator() = delete;
        ~SurfaceConfigurator() = delete;

        SurfaceConfigurator(const SurfaceConfigurator&) = delete;
        SurfaceConfigurator(SurfaceConfigurator&&) = delete;

        SurfaceConfigurator& operator=(const SurfaceConfigurator&) = delete;
        SurfaceConfigurator& operator=(SurfaceConfigurator&&) = delete;

        static void Configure(WGPUSurface surface, WGPUAdapter adapter, WGPUDevice device,
                                     const SurfaceConfiguration& configuration);
        static inline void Unconfigure(WGPUSurface surface);
    };

#include "SurfaceConfigurator.inl"
}
