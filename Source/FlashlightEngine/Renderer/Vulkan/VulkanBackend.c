// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.h

#include "FlashlightEngine/Renderer/Vulkan/VulkanBackend.h"

#include "FlashlightEngine/Renderer/Vulkan/VulkanTypes.inl"

#include "FlashlightEngine/Core/Logger.h"

static FlVulkanContext Context;

FlBool8 flVulkanRendererBackendInitialize(FlRendererBackend* backend, const char* applicationName, struct FlPlatformState* platformState) {
    // TODO: Custom allocator.
    Context.Allocator = 0;

    // Vulkan instance
    VkApplicationInfo applicationInfo = {VK_STRUCTURE_TYPE_APPLICATION_INFO};
    applicationInfo.apiVersion = VK_API_VERSION_1_2;
    applicationInfo.pApplicationName = applicationName;
    applicationInfo.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
    applicationInfo.pEngineName = "Flashlight Engine";
    applicationInfo.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);

    VkInstanceCreateInfo instanceInfo = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
    instanceInfo.pApplicationInfo = &applicationInfo;
    instanceInfo.enabledExtensionCount = 0;
    instanceInfo.ppEnabledExtensionNames = 0;
    instanceInfo.enabledLayerCount = 0;
    instanceInfo.ppEnabledLayerNames = 0;

    VkResult result = vkCreateInstance(&instanceInfo, Context.Allocator, &Context.Instance);

    if (result != VK_SUCCESS) {
        FL_LOG_ERROR("vkCreateInstance failed with result: %u", result);
        return FALSE;
    }

    FL_LOG_INFO("Vulkan renderer initialized successfully.")
    return TRUE;
}

void flVulkanRendererBackendShutdown(FlRendererBackend* backend) {
    vkDestroyInstance(Context.Instance, Context.Allocator);
}

void flVulkanRendererBackendOnResize(FlRendererBackend* backend, FlUInt16 width, FlUInt16 height) {

}

FlBool8 flVulkanRendererBackendBeginFrame(FlRendererBackend* backend, FlFloat32 deltaTime) {
    return TRUE;
}

FlBool8 flVulkanRendererBackendEndFrame(FlRendererBackend* backend, FlFloat32 deltaTime) {
    return TRUE;
}