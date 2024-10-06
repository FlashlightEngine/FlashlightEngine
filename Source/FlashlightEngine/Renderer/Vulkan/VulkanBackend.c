// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.h

#include "FlashlightEngine/Renderer/Vulkan/VulkanBackend.h"

#include "FlashlightEngine/Renderer/Vulkan/VulkanTypes.inl"
#include "FlashlightEngine/Renderer/Vulkan/VulkanPlatform.h"
#include "FlashlightEngine/Renderer/Vulkan/VulkanDevice.h"
#include "FlashlightEngine/Renderer/Vulkan/VulkanSwapchain.h"
#include "FlashlightEngine/Renderer/Vulkan/VulkanRenderPass.h"
#include "FlashlightEngine/Renderer/Vulkan/VulkanCommandBuffer.h"
#include "FlashlightEngine/Renderer/Vulkan/VulkanFramebuffer.h"
#include "FlashlightEngine/Renderer/Vulkan/VulkanFence.h"
#include "FlashlightEngine/Renderer/Vulkan/VulkanUtils.h"

#include "FlashlightEngine/Core/Logger.h"
#include "FlashlightEngine/Core/FlString.h"
#include "FlashlightEngine/Core/FlMemory.h"
#include "FlashlightEngine/Core/Application.h"

#include "FlashlightEngine/Containers/DArray.h"

static FlVulkanContext Context;
static FlUInt32 CachedFramebufferWidth = 0;
static FlUInt32 CachedFramebufferHeight = 0;

VKAPI_ATTR VkBool32 VKAPI_CALL flVkDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                VkDebugUtilsMessageTypeFlagsEXT messageTypes,
                                                const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                void* pUserData);

FlInt32 flVulkanRendererBackendFindMemoryIndex(FlUInt32 typeFilter, FlUInt32 propertyFlags);

void flVulkanRendererBackendCreateCommandBuffers(FlRendererBackend* backend);
void flVulkanRendererBackendRegenerateFramebuffers(FlRendererBackend* backend, FlVulkanSwapchain* swapchain, FlVulkanRenderPass* renderPass);
FlBool8 flVulkanRendererBackendRecreateSwapchain(FlRendererBackend* backend);

FlBool8 flVulkanRendererBackendInitialize(FlRendererBackend* backend, const char* applicationName, struct FlPlatformState* platformState) {
    Context.FindMemoryIndex = flVulkanRendererBackendFindMemoryIndex;
    
    // TODO: Custom allocator.
    Context.Allocator = 0;

    flApplicationGetFramebufferSize(&CachedFramebufferWidth, &CachedFramebufferHeight);
    Context.FramebufferWidth = (CachedFramebufferWidth != 0) ? CachedFramebufferWidth : 800;
    Context.FramebufferHeight = (CachedFramebufferHeight != 0) ? CachedFramebufferHeight : 600;
    CachedFramebufferWidth = 0;
    CachedFramebufferHeight = 0;

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
    FL_LOG_DEBUG("Validation layers enabled. Enumerating.")

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
        FL_LOG_DEBUG("Searching for layer: %s...", requiredValidationLayerNames[i]);
        FlBool8 found = FALSE;
        for (FlUInt32 j = 0; j < availableLayerCount; ++j) {
            if (flStringsEqual(requiredValidationLayerNames[i], availableLayers[j].layerName)) {
                found = TRUE;
                FL_LOG_DEBUG("Found.")
                break;
            }
        }

        if (!found) {
            FL_LOG_FATAL("Required validation layer is missing: %s", requiredValidationLayerNames[i]);
            return FALSE;
        }
    }

    FL_LOG_DEBUG("All required validation layers are present.")

    flDArrayDestroy(availableLayers);
#endif

    instanceInfo.enabledLayerCount = requiredValidationLayerCount;
    instanceInfo.ppEnabledLayerNames = requiredValidationLayerNames;

    VK_CHECK(vkCreateInstance(&instanceInfo, Context.Allocator, &Context.Instance));
    FL_LOG_DEBUG("Vulkan Instance created.")

    flDArrayDestroy(requiredExtensions);
    
#ifdef FL_DEBUG
    flDArrayDestroy(requiredValidationLayerNames);
#endif

// Create the debug messenger.
#ifdef FL_DEBUG
    FL_LOG_DEBUG("Creating Vulkan debug messenger...")
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
    FL_LOG_DEBUG("Vulkan debug messenger created.")
#endif

    // Surface creation.
    FL_LOG_DEBUG("Creating Vulkan surface...")
    if (!flPlatformCreateVulkanSurface(platformState, &Context)) {
        FL_LOG_FATAL("Failed to create Vulkan surface.");
        return FALSE;
    }
    FL_LOG_DEBUG("Vulkan surface created.")

    // Device creation.
    if (!flVulkanDeviceCreate(&Context)) {
        FL_LOG_FATAL("Failed to create Vulkan device.");
        return FALSE;
    }

    // Swapchain creation.
    FL_LOG_DEBUG("Creating Vulkan swapchain...")
    flVulkanSwapchainCreate(&Context, Context.FramebufferWidth, Context.FramebufferHeight, &Context.Swapchain);

    // Render pass creation.
    FL_LOG_DEBUG("Creating main Vulkan render pass...")
    flVulkanRenderPassCreate(
        &Context,
        &Context.MainRenderPass,
        0, 0, Context.FramebufferWidth, Context.FramebufferHeight,
        0.0f, 0.0f, 0.2f, 1.0f,
        1.0f,
        0
    );
    FL_LOG_DEBUG("Main Vulkan render pass created.")

    // Swapchain framebuffers.
    FL_LOG_DEBUG("Creating Vulkan framebuffers...")
    Context.Swapchain.Framebuffers = flDArrayReserve(FlVulkanFramebuffer, Context.Swapchain.ImageCount);
    flVulkanRendererBackendRegenerateFramebuffers(backend, &Context.Swapchain, &Context.MainRenderPass);
    FL_LOG_DEBUG("Vulkan framebuffers created.")
    
    FL_LOG_DEBUG("Creating Vulkan command buffers...")
    flVulkanRendererBackendCreateCommandBuffers(backend);
    FL_LOG_DEBUG("Vulkan command buffers created.")

    FL_LOG_DEBUG("Creating Vulkan synchronisation objects...")
    Context.ImageAvailableSemaphores = flDArrayReserve(VkSemaphore, Context.Swapchain.MaxFramesInFlight);
    Context.QueueCompleteSemaphores = flDArrayReserve(VkSemaphore, Context.Swapchain.MaxFramesInFlight);
    Context.InFlightFences = flDArrayReserve(FlVulkanFence, Context.Swapchain.MaxFramesInFlight);

    for (FlUInt8 i = 0; i < Context.Swapchain.MaxFramesInFlight; ++i) {
        VkSemaphoreCreateInfo semaphoreCreateInfo = {VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
        vkCreateSemaphore(Context.Device.LogicalDevice, &semaphoreCreateInfo, Context.Allocator, &Context.ImageAvailableSemaphores[i]);
        vkCreateSemaphore(Context.Device.LogicalDevice, &semaphoreCreateInfo, Context.Allocator, &Context.QueueCompleteSemaphores[i]);

        // Create the fence in a signealed state, indicating that the first frame has already been "rendered".
        // This will prevent the application from waiting indefinitely for the first frame to render since it
        // cannot be rendered util a frame is "rendered" before it.
        flVulkanFenceCreate(&Context, TRUE, &Context.InFlightFences[i]);
    }

    // In flight fences should not yet exist at this point, so clear the list. These are stored in pointers
    // because the initial state should be 0, and will be 0 when not in use. Acutal fences are not owned 
    // by this list.
    Context.ImagesInFlight = flDArrayReserve(FlVulkanFence, Context.Swapchain.ImageCount);
    for (FlUInt32 i = 0; i < Context.Swapchain.ImageCount; ++i) {
        Context.ImagesInFlight[i] = 0;
    }
    FL_LOG_DEBUG("Vulkan synchronisation objects created.")

    FL_LOG_INFO("Vulkan renderer backend initialized successfully.")
    return TRUE;
}

void flVulkanRendererBackendShutdown(FlRendererBackend* backend) {
    FL_UNUSED(backend);

    vkDeviceWaitIdle(Context.Device.LogicalDevice);

    FL_LOG_DEBUG("Destroying Vulkan synchronisation objects...")
    for (FlUInt8 i = 0; i < Context.Swapchain.MaxFramesInFlight; ++i) {
        if (Context.ImageAvailableSemaphores[i]) {
            vkDestroySemaphore(Context.Device.LogicalDevice, Context.ImageAvailableSemaphores[i], Context.Allocator);
        }

        if (Context.QueueCompleteSemaphores[i]) {
            vkDestroySemaphore(Context.Device.LogicalDevice, Context.QueueCompleteSemaphores[i], Context.Allocator);
        }

        flVulkanFenceDestroy(&Context, &Context.InFlightFences[i]);
    }
    flDArrayDestroy(Context.ImageAvailableSemaphores);
    Context.ImageAvailableSemaphores = 0;

    flDArrayDestroy(Context.QueueCompleteSemaphores);
    Context.QueueCompleteSemaphores = 0;

    flDArrayDestroy(Context.InFlightFences);
    Context.InFlightFences = 0;

    flDArrayDestroy(Context.ImagesInFlight);
    Context.ImagesInFlight = 0;

    FL_LOG_DEBUG("Destroying Vulkan command buffers...")
    for (FlUInt32 i = 0; i < Context.Swapchain.ImageCount; ++i) {
        if (Context.GraphicsCommandBuffers[i].Handle) {
            flVulkanCommandBufferFree(&Context, Context.Device.GraphicsCommandPool, &Context.GraphicsCommandBuffers[i]);
            Context.GraphicsCommandBuffers[i].Handle = VK_NULL_HANDLE;
        }
    }
    flDArrayDestroy(Context.GraphicsCommandBuffers);
    Context.GraphicsCommandBuffers = 0;

    FL_LOG_DEBUG("Destroying Vulkan framebuffers...")
    for (FlUInt32 i = 0; i < Context.Swapchain.ImageCount; ++i) {
        flVulkanFramebufferDestroy(&Context, &Context.Swapchain.Framebuffers[i]);
    }
    flDArrayDestroy(Context.Swapchain.Framebuffers);
    Context.Swapchain.Framebuffers = 0;

    FL_LOG_DEBUG("Destroying Vulkan main render pass...")
    flVulkanRenderPassDestroy(&Context, &Context.MainRenderPass);

    FL_LOG_DEBUG("Destroying Vulkan swapchain...")
    flVulkanSwapchainDestroy(&Context, &Context.Swapchain);

    FL_LOG_DEBUG("Destroying Vulkan device...")
    flVulkanDeviceDestroy(&Context);

#ifdef FL_DEBUG
    if (Context.DebugMessenger) {
        FL_LOG_DEBUG("Destroying Vulkan debug messenger...")
        PFN_vkDestroyDebugUtilsMessengerEXT func = 
            (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(Context.Instance, "vkDestroyDebugUtilsMessengerEXT");
        func (Context.Instance, Context.DebugMessenger, Context.Allocator);
    }
#endif

    if (Context.Surface) {
        FL_LOG_DEBUG("Destroying Vulkan surface...")
        vkDestroySurfaceKHR(Context.Instance, Context.Surface, Context.Allocator);
    }
    
    FL_LOG_DEBUG("Destroying Vulkan instance...")
    vkDestroyInstance(Context.Instance, Context.Allocator);
}

void flVulkanRendererBackendOnResize(FlRendererBackend* backend, FlUInt16 width, FlUInt16 height) {
    // Update the "framebuffer size generation", a counter which indicates when the
    // framebuffer size has been updated.
    CachedFramebufferWidth = width;
    CachedFramebufferHeight = height;
    Context.FramebufferSizeGeneration++;

    FL_LOG_DEBUG("Vulkan renderer backend->Resized: w/h/gen: %i/%i/%llu", width, height, Context.FramebufferSizeGeneration);
}

FlBool8 flVulkanRendererBackendBeginFrame(FlRendererBackend* backend, FlFloat32 deltaTime) {
    FlVulkanDevice* device = &Context.Device;

    // Check if recreating swapchain and boot out.
    if (Context.RecreatingSwapchain) {
        VkResult result = vkDeviceWaitIdle(device->LogicalDevice);
        if (!flVulkanResultIsSuccess(result)) {
            FL_LOG_ERROR("flVulkanRendererBackendBeginFrame vkDeviceWaitIdle (1) failed: '%s'", flVulkanResultToString(result, TRUE))
            return FALSE;
        }
        FL_LOG_INFO("Currently recreating swapchain, booting out of frame.")
        return FALSE;
    }

    // Check if the framebuffer has been resized. If so, a new swapchain must be created.
    if (Context.FramebufferSizeGeneration != Context.FramebufferSizeLastGeneration) {
        VkResult result = vkDeviceWaitIdle(device->LogicalDevice);
        if (!flVulkanResultIsSuccess(result)) {
            FL_LOG_ERROR("flVulkanRendererBackendBeginFrame vkDeviceWaitIdle (2) failed: '%s'", flVulkanResultToString(result, TRUE))
            return FALSE;
        }
        
        // If the swapchain recreation failed (because, for example, the window was minimized),
        // boot out before unsetting the flag.
        if (!flVulkanRendererBackendRecreateSwapchain(backend)) {
            return FALSE;
        }

        FL_LOG_INFO("Resized, booting out of frame.")
        return FALSE;
    }

    // Wait for the execution of the current frame to complete. The fence being free will allow this one to move on.
    if (!flVulkanFenceWait(
        &Context,
        &Context.InFlightFences[Context.CurrentFrame],
        UINT64_MAX
    )) {
        FL_LOG_WARN("In-flight fence wait failure.")
        return FALSE;
    }

    // Acquire the next image from the swapchain. Pass along the semaphore that should be signaled when this completes.
    // This same semaphore will later be waited on by the queue submission to ensure the image is available.
    if (!flVulkanSwapchainAcquireNextImageIndex(
        &Context,
        &Context.Swapchain,
        UINT64_MAX,
        Context.ImageAvailableSemaphores[Context.CurrentFrame],
        0,
        &Context.ImageIndex
    )) {
        return FALSE;
    }

    // Begin recording commands.
    FlVulkanCommandBuffer* commandBuffer = &Context.GraphicsCommandBuffers[Context.ImageIndex];
    flVulkanCommandBufferReset(commandBuffer);
    flVulkanCommandBufferBegin(commandBuffer, FALSE, FALSE, FALSE);

    // Dynamic states
    VkViewport viewport;
    viewport.x = 0.0f;
    viewport.y = (FlFloat32)Context.FramebufferHeight;
    viewport.width = (FlFloat32)Context.FramebufferWidth;
    viewport.height = (FlFloat32)Context.FramebufferHeight;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor;
    scissor.offset.x = scissor.offset.y = 0;
    scissor.extent.width = Context.FramebufferWidth;
    scissor.extent.height = Context.FramebufferHeight;

    vkCmdSetViewport(commandBuffer->Handle, 0, 1, &viewport);
    vkCmdSetScissor(commandBuffer->Handle, 0, 1, &scissor);

    Context.MainRenderPass.W = Context.FramebufferWidth;
    Context.MainRenderPass.H = Context.FramebufferHeight;

    // Begin the render pass.
    flVulkanRenderPassBegin(commandBuffer, &Context.MainRenderPass, Context.Swapchain.Framebuffers[Context.ImageIndex].Handle);

    return TRUE;
}

FlBool8 flVulkanRendererBackendEndFrame(FlRendererBackend* backend, FlFloat32 deltaTime) {
    FlVulkanCommandBuffer* commandBuffer = &Context.GraphicsCommandBuffers[Context.ImageIndex];

    flVulkanRenderPassEnd(commandBuffer, &Context.MainRenderPass);

    flVulkanCommandBufferEnd(commandBuffer);
    
    // Make sure the previous frame is not using this image (i.e. its fence is being waited on).
    if (Context.ImagesInFlight[Context.ImageIndex] != VK_NULL_HANDLE) {
        flVulkanFenceWait(&Context, Context.ImagesInFlight[Context.ImageIndex], UINT64_MAX);
    }

    // Mark the image fence as in-use by this frame.
    Context.ImagesInFlight[Context.ImageIndex] = &Context.InFlightFences[Context.CurrentFrame];

    // Reset the fence for use on the next frame.
    flVulkanFenceReset(&Context, &Context.InFlightFences[Context.CurrentFrame]);

    VkSubmitInfo submitInfo = {VK_STRUCTURE_TYPE_SUBMIT_INFO};
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer->Handle;

    // The semaphore(s) to be signaled when the queue is complete.
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &Context.QueueCompleteSemaphores[Context.CurrentFrame];

    // Wait semaphores ensure that the operation cannot begin until the image is available.
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &Context.ImageAvailableSemaphores[Context.CurrentFrame];

    // Each semaphore waits on the corresponding pipeline stage to complete. 1:1 ratio.
    // VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT prevents subsequent color attachment
    // writes from executing until the semaphore signals (i.e. one frame is preseted at a time).
    VkPipelineStageFlags flags[1] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.pWaitDstStageMask = flags;

    VkResult result = vkQueueSubmit(
        Context.Device.GraphicsQueue,
        1,
        &submitInfo,
        Context.InFlightFences[Context.CurrentFrame].Handle
    );

    if (result != VK_SUCCESS) {
        FL_LOG_ERROR("vkQueueSubmit failed with result: %s", flVulkanResultToString(result, TRUE))
        return FALSE;
    }

    flVulkanCommandBufferUpdateSubmitted(commandBuffer);

    flVulkanSwapchainPresent(
        &Context,
        &Context.Swapchain,
        Context.Device.GraphicsQueue,
        Context.Device.PresentQueue,
        Context.QueueCompleteSemaphores[Context.CurrentFrame],
        Context.ImageIndex
    );

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

FlInt32 flVulkanRendererBackendFindMemoryIndex(FlUInt32 typeFilter, FlUInt32 propertyFlags) {
    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(Context.Device.PhysicalDevice, &memoryProperties);

    for (FlUInt32 i = 0; i < memoryProperties.memoryTypeCount; ++i) {
        // Check each memory type to see if its bit is set to one.
        if (typeFilter & (1 << i) && (memoryProperties.memoryTypes[i].propertyFlags & propertyFlags) == propertyFlags) {
            return i;
        }
    }

    FL_LOG_WARN("Unable to find a suitable memory type.")
    return -1;
}

void flVulkanRendererBackendCreateCommandBuffers(FlRendererBackend* backend) {
    if (!Context.GraphicsCommandBuffers) {
        Context.GraphicsCommandBuffers = flDArrayReserve(FlVulkanCommandBuffer, Context.Swapchain.ImageCount);
        for (FlUInt32 i = 0; i < Context.Swapchain.ImageCount; ++i) {
            flZeroMemory(&Context.GraphicsCommandBuffers[i], sizeof(FlVulkanCommandBuffer));
        }
    }

    
    for (FlUInt32 i = 0; i < Context.Swapchain.ImageCount; ++i) {
        if (Context.GraphicsCommandBuffers[i].Handle) {
            flVulkanCommandBufferFree(&Context, Context.Device.GraphicsCommandPool, &Context.GraphicsCommandBuffers[i]);
        }
        flZeroMemory(&Context.GraphicsCommandBuffers[i], sizeof(FlVulkanCommandBuffer));
        flVulkanCommandBufferAllocate(&Context, Context.Device.GraphicsCommandPool, TRUE, &Context.GraphicsCommandBuffers[i]);
    }
}

void flVulkanRendererBackendRegenerateFramebuffers(FlRendererBackend* backend, FlVulkanSwapchain* swapchain, FlVulkanRenderPass* renderPass) {
    for (FlUInt32 i = 0; i < swapchain->ImageCount; ++i) {
        // TODO: Make this dynamic based on the currently configured attachments.
        FlUInt32 attachmentCount = 2;
        VkImageView attachments[] = {
            swapchain->ImageViews[i],
            swapchain->DepthAttachment.ImageView
        };

        flVulkanFramebufferCreate(
            &Context,
            renderPass,
            Context.FramebufferWidth,
            Context.FramebufferHeight,
            attachmentCount,
            attachments,
            &Context.Swapchain.Framebuffers[i]
        );
    }
}

FlBool8 flVulkanRendererBackendRecreateSwapchain(FlRendererBackend* backend) {
    // If already being recreated, do not try again.
    if (Context.RecreatingSwapchain) {
        FL_LOG_DEBUG("flVulkanRendererBackendRecreateSwapchain called when already recreating. Booting out.")
        return FALSE;
    }

    // Detect if the window is too small to be drawn to.
    if (Context.FramebufferWidth == 0 || Context.FramebufferHeight == 0) {
        FL_LOG_DEBUG("flVulkanRendererBackendRecreateSwapchain called when window is < 1 in a dimension. Booting out.")
        return FALSE;
    }

    // Mark as recreating if the dimensions are valid.
    Context.RecreatingSwapchain = TRUE;

    // Wait for any operations to complete.
    vkDeviceWaitIdle(Context.Device.LogicalDevice);

    // Clear these out just in case.
    for (FlUInt32 i = 0; i < Context.Swapchain.ImageCount; ++i) {
        Context.ImagesInFlight[i] = 0;
    }

    // Requery support.
    flVulkanDeviceQuerySwapchainSupport(Context.Device.PhysicalDevice, Context.Surface, &Context.Device.SwapchainSupport);
    flVulkanDeviceDetectDepthFormat(&Context.Device);

    flVulkanSwapchainRecreate(&Context, CachedFramebufferWidth, CachedFramebufferHeight, &Context.Swapchain);

    // Sync the framebuffer size with the cached sizes.
    Context.FramebufferWidth = CachedFramebufferWidth;
    Context.FramebufferHeight = CachedFramebufferHeight;
    Context.MainRenderPass.W = Context.FramebufferWidth;
    Context.MainRenderPass.H = Context.FramebufferHeight;
    CachedFramebufferWidth = 0;
    CachedFramebufferHeight = 0;

    // Update the framebuffer size generation.
    Context.FramebufferSizeLastGeneration = Context.FramebufferSizeGeneration;

    // Cleanup swapchain
    for (FlUInt32 i = 0; i < Context.Swapchain.ImageCount; ++i) {
        flVulkanCommandBufferFree(&Context, Context.Device.GraphicsCommandPool, &Context.GraphicsCommandBuffers[i]);

        // Framebuffers
        flVulkanFramebufferDestroy(&Context, &Context.Swapchain.Framebuffers[i]);
    } 

    Context.MainRenderPass.X = 0;
    Context.MainRenderPass.Y = 0;
    Context.MainRenderPass.W = Context.FramebufferWidth;
    Context.MainRenderPass.H = Context.FramebufferHeight;

    flVulkanRendererBackendRegenerateFramebuffers(backend, &Context.Swapchain, &Context.MainRenderPass);

    flVulkanRendererBackendCreateCommandBuffers(backend);

    // Clear the recreating flag
    Context.RecreatingSwapchain = FALSE;

    return TRUE;
}
