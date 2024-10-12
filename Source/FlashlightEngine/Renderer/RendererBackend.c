// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.h

#include "FlashlightEngine/Renderer/RendererBackend.h"

#include "FlashlightEngine/Renderer/Vulkan/VulkanBackend.h"

#include "FlashlightEngine/Core/Logger.h"

FlBool8 flRendererBackendCreate(FlRendererBackendType type, struct FlPlatformState* platformState, FlRendererBackend* outRenderBackend) {
    outRenderBackend->PlatformState = platformState;

    if (type == FlRendererBackendVulkan) {
        outRenderBackend->Initialize = flVulkanRendererBackendInitialize;
        outRenderBackend->Shutdown = flVulkanRendererBackendShutdown;
        outRenderBackend->BeginFrame = flVulkanRendererBackendBeginFrame;
        outRenderBackend->EndFrame = flVulkanRendererBackendEndFrame;
        outRenderBackend->Resized = flVulkanRendererBackendOnResize;

        return true;
    }

    return false;
}

void flRendererBackendDestroy(FlRendererBackend* rendererBackend) {
    rendererBackend->Initialize = 0;
    rendererBackend->Shutdown   = 0;
    rendererBackend->BeginFrame = 0;
    rendererBackend->EndFrame   = 0;
    rendererBackend->Resized    = 0;
}
