// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.h

#include "FlashlightEngine/Renderer/Vulkan/VulkanBackend.h"

#include "FlashlightEngine/Renderer/Vulkan/VulkanTypes.inl"
#include "FlashlightEngine/Renderer/Vulkan/VulkanPlatform.h"
#include "FlashlightEngine/Renderer/Vulkan/VulkanDevice.h"

#include "FlashlightEngine/Core/Logger.h"
#include "FlashlightEngine/Core/FlString.h"

#include "FlashlightEngine/Containers/DArray.h"

static FlVulkanContext Context;

VKAPI_ATTR VkBool32 VKAPI_CALL flVkDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                VkDebugUtilsMessageTypeFlagsEXT messageTypes,
                                                const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                void* pUserData);

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

    FL_LOG_DEBUG("Required Vulkan instance extensions:");
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
    VK_CHECK(vkEnumerateInstanceLayerProperties(&availableLayerCount, 0))
    VkLayerProperties* availableLayers = flDArrayReserve(VkLayerProperties, availableLayerCount);
    VK_CHECK(vkEnumerateInstanceLayerProperties(&availableLayerCount, availableLayers))

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

    flDArrayDestroy(availableLayers);
#endif

    instanceInfo.enabledLayerCount = requiredValidationLayerCount;
    instanceInfo.ppEnabledLayerNames = requiredValidationLayerNames;

    VK_CHECK(vkCreateInstance(&instanceInfo, Context.Allocator, &Context.Instance));
    FL_LOG_INFO("Vulkan Instance created.")

    flDArrayDestroy(requiredExtensions);
    
#ifdef FL_DEBUG
    flDArrayDestroy(requiredValidationLayerNames);
#endif

// Create the debug messenger.
#ifdef FL_DEBUG
    FL_LOG_INFO("Creating Vulkan debug messenger...")
    FlUInt32 logSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT   |
                           VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;   // |
                           // VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT};
    debugCreateInfo.messageSeverity = logSeverity;
    debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT     |
                                  VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
                                  VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
    debugCreateInfo.pfnUserCallback = flVkDebugCallback;

    PFN_vkCreateDebugUtilsMessengerEXT func = 
        (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(Context.Instance, "vkCreateDebugUtilsMessengerEXT");
    FL_ASSERT_MSG(func, "Failed to create debug messenger.")
    VK_CHECK(func(Context.Instance, &debugCreateInfo, Context.Allocator, &Context.DebugMessenger))
    FL_LOG_INFO("Vulkan debug messenger created.")
#endif

    // Surface creation.
    FL_LOG_INFO("Creating Vulkan surface...")
    if (!flPlatformCreateVulkanSurface(platformState, &Context)) {
        FL_LOG_FATAL("Failed to create Vulkan surface.");
        return FALSE;
    }
    FL_LOG_INFO("Vulkan surface created.")

    // Device creation.
    if (!flVulkanDeviceCreate(&Context)) {
        FL_LOG_FATAL("Failed to create Vulkan device.");
        return FALSE;
    }

    FL_LOG_INFO("Vulkan renderer initialized successfully.")
    return TRUE;
}

void flVulkanRendererBackendShutdown(FlRendererBackend* backend) {
    flVulkanDeviceDestroy(&Context);

#ifdef FL_DEBUG
    if (Context.DebugMessenger) {
        FL_LOG_INFO("Destroying Vulkan debug messenger...")
        PFN_vkDestroyDebugUtilsMessengerEXT func = 
            (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(Context.Instance, "vkDestroyDebugUtilsMessengerEXT");
        func (Context.Instance, Context.DebugMessenger, Context.Allocator);
    }
#endif

    if (Context.Surface) {
        FL_LOG_INFO("Destroying Vulkan surface...")
        vkDestroySurfaceKHR(Context.Instance, Context.Surface, Context.Allocator);
    }
    
    FL_LOG_INFO("Destroying Vulkan instance...")
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

VKAPI_ATTR VkBool32 VKAPI_CALL flVkDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                VkDebugUtilsMessageTypeFlagsEXT messageTypes,
                                                const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                void* pUserData) {
    switch (messageSeverity) {
        default:
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            FL_LOG_ERROR("[Validation Layers][ERROR] %s", pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            FL_LOG_WARN("[Validation Layers][WARNING] %s", pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            FL_LOG_INFO("[Validation Layers][INFO] %s", pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            FL_LOG_TRACE("[Validation Layers][VERBOSE] %s", pCallbackData->pMessage);
            break;
    }

    // Always return VK_FALSE, we don't want to abort the Vulkan call.
    return VK_FALSE;
}