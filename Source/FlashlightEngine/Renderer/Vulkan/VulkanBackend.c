// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.h

#include "FlashlightEngine/Renderer/Vulkan/VulkanBackend.h"

#include "FlashlightEngine/Renderer/Vulkan/VulkanTypes.inl"
#include "FlashlightEngine/Renderer/Vulkan/VulkanPlatform.h"

#include "FlashlightEngine/Core/Logger.h"
#include "FlashlightEngine/Core/FlString.h"

#include "FlashlightEngine/Containers/DArray.h"

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

    // Obtain a list of required extensions
    const char** requiredExtensions = flDArrayCreate(const char*);
    flDArrayPush(requiredExtensions, &VK_KHR_SURFACE_EXTENSION_NAME); // Generic surface extension.

    flPlatformGetRequiredExtensionNames(&requiredExtensions);         // Platform specific extension(s).

#ifdef FL_DEBUG
    flDArrayPush(requiredExtensions, &VK_EXT_DEBUG_UTILS_EXTENSION_NAME); // Debug utilities.

    FL_LOG_DEBUG("Required Vulkan extensions:");
    FlUInt32 length = flDArrayLength(requiredExtensions);
    for (FlUInt32 i = 0; i < length; ++i) {
        FL_LOG_DEBUG("\t- %s", requiredExtensions[i]);
    }
#endif

    instanceInfo.enabledExtensionCount = flDArrayLength(requiredExtensions);
    instanceInfo.ppEnabledExtensionNames = requiredExtensions;

    // Validation layers.
    const char** requiredValidationLayerNames = 0;
    FlUInt32 requiredValidationLayerCount = 0;

// If validation should be done, get a list of the required validation layers names
// and make sure they exist. Validation layers should only be enabled on non-release builds.
#ifdef FL_DEBUG
    FL_LOG_INFO("Validation layers enabled. Enumerating.")

    // The list of validation layers required.
    requiredValidationLayerNames = flDArrayCreate(const char*);
    flDArrayPush(requiredValidationLayerNames, &"VK_LAYER_KHRONOS_validation");
    requiredValidationLayerCount = flDArrayLength(requiredValidationLayerNames);

    // Obtain a list of available validation layers.
    FlUInt32 availableLayerCount = 0;
    VK_CHECK(vkEnumerateInstanceLayerProperties(&availableLayerCount, 0));
    VkLayerProperties* availableLayers = flDArrayReserve(VkLayerProperties, availableLayerCount);
    VK_CHECK(vkEnumerateInstanceLayerProperties(&availableLayerCount, availableLayers));

    // Verify all layers are available.
    for (FlUInt32 i = 0; i < requiredValidationLayerCount; ++i) {
        FL_LOG_INFO("Searching for layer: %s...", requiredValidationLayerNames[i]);
        FlBool8 found = FALSE;
        for (FlUInt32 j = 0; j < availableLayerCount; ++j) {
            if (flStringsEqual(requiredValidationLayerNames[i], availableLayers[j].layerName)) {
                found = TRUE;
                FL_LOG_INFO("Found.")
                break;
            }
        }

        if (!found) {
            FL_LOG_FATAL("Required validation layer is missing: %s", requiredValidationLayerNames[i]);
            return FALSE;
        }
    }

    FL_LOG_INFO("All required validation layers are present.")
#endif

    instanceInfo.enabledLayerCount = requiredValidationLayerCount;
    instanceInfo.ppEnabledLayerNames = requiredValidationLayerNames;

    VK_CHECK(vkCreateInstance(&instanceInfo, Context.Allocator, &Context.Instance));

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