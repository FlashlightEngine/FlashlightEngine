/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : SurfaceConfigurator.cpp
 * Description : Definitions of methods from the SurfaceConfigurator class.
 */
#include "FlashlightEngine/Renderer/WGPUWrapper/SurfaceConfigurator.hpp"

namespace Flashlight::WGPUWrapper {
    void SurfaceConfigurator::Configure(WGPUSurface surface, WGPUAdapter adapter,
                                               WGPUDevice device, const SurfaceConfiguration& configuration) {
        WGPUSurfaceConfiguration surfaceConfiguration = {};
        surfaceConfiguration.nextInChain = nullptr;

        surfaceConfiguration.width = configuration.Width;
        surfaceConfiguration.height = configuration.Height;

        WGPUTextureFormat const surfaceFormet = wgpuSurfaceGetPreferredFormat(surface, adapter);
        surfaceConfiguration.format = surfaceFormet;
        // We do not need any particular view format.
        surfaceConfiguration.viewFormatCount = 0;
        surfaceConfiguration.viewFormats = nullptr;

        surfaceConfiguration.usage = WGPUTextureUsage_RenderAttachment;

        surfaceConfiguration.device = device;

        surfaceConfiguration.presentMode = WGPUPresentMode_Fifo;

        surfaceConfiguration.alphaMode = WGPUCompositeAlphaMode_Auto;

        wgpuSurfaceConfigure(surface, &surfaceConfiguration);
    }
}