// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.h

#include "FlashlightEngine/Renderer/RendererFrontend.h"

#include "FlashlightEngine/Renderer/RendererBackend.h"

#include "FlashlightEngine/Core/Logger.h"
#include "FlashlightEngine/Core/FlMemory.h"

// Backend render context.
static FlRendererBackend* Backend = 0;

FlBool8 flRendererInitialize(const char* applicationName, struct FlPlatformState* platformState) {
    Backend = flAllocate(sizeof(FlRendererBackend), FlMemoryTagRenderer);

    // TODO: Make this configurable.
    flRendererBackendCreate(FlRendererBackendVulkan, platformState, Backend);
    Backend->FrameNumber = 0;

    if (!Backend->Initialize(Backend, applicationName, platformState)) {
        FL_LOG_FATAL("Renderer backend failed to initialize. Shutting down.")
        return FALSE;
    }

    FL_LOG_INFO("Renderer subsystem initialized.")

    return TRUE;
}

void flRendererShutdown() {
    Backend->Shutdown(Backend);
    flFree(Backend, sizeof(FlRendererBackend), FlMemoryTagRenderer);
}

void flRendererOnResize(FlUInt16 width, FlUInt16 height) {

}

FlBool8 flRendererBeginFrame(FlFloat32 deltaTime) {
    return Backend->BeginFrame(Backend, deltaTime);
}

FlBool8 flRendererEndFrame(FlFloat32 deltaTime) {
    FlBool8 result = Backend->EndFrame(Backend, deltaTime);
    Backend->FrameNumber++;
    return result;
}

FlBool8 flRendererDrawFrame(FlRenderPacket* packet) {
    // If the begin frame returned successfully, mid-frame operations may continue.
    if (flRendererBeginFrame(packet->DeltaTime)) {
        
        // End the frame. If this fails, it is likely unrecoverable.
        FlBool8 result = flRendererEndFrame(packet->DeltaTime);

        if (!result) {
            FL_LOG_ERROR("flRendererEndFrame failed. Shutting down application.")
            return FALSE;
        } 
    }

    return TRUE; 
}