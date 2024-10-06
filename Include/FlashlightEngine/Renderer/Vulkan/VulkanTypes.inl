// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.h

#pragma once

#ifndef FL_RENDERER_VULKAN_VULKANTYPES_INL
#define FL_RENDERER_VULKAN_VULKANTYPES_INL

#include "FlashlightEngine/Prerequisites.h"
#include "FlashlightEngine/Export.h"

#include "FlashlightEngine/Core/Asserts.h"

#include <vulkan/vulkan.h>

// Check the given expression's result return value against VK_SUCCESS.
#define VK_CHECK(expr)                                   \
    {                                                    \
        if ((expr) != VK_SUCCESS) {                      \
            FL_ASSERT_MSG(FALSE, "Vulkan call failed."); \
        }                                                \
    }

typedef struct FlVulkanSwapchainSupport {
    VkSurfaceCapabilitiesKHR Capabilities;
    FlUInt32 FormatCount;
    VkSurfaceFormatKHR* Formats;
    FlUInt32 PresentModeCount;
    VkPresentModeKHR* PresentModes;
} FlVulkanSwapchainSupport;

typedef struct FlVulkanDevice {
    VkPhysicalDevice PhysicalDevice;
    VkDevice LogicalDevice;
    FlVulkanSwapchainSupport SwapchainSupport;

    FlUInt32 GraphicsQueueIndex;
    FlUInt32 PresentQueueIndex;
    FlUInt32 TransferQueueIndex;

    VkQueue GraphicsQueue;
    VkQueue PresentQueue;
    VkQueue TransferQueue;

    VkCommandPool GraphicsCommandPool;

    VkPhysicalDeviceProperties Properties;
    VkPhysicalDeviceFeatures Features;
    VkPhysicalDeviceMemoryProperties MemoryProperties;

    VkFormat DepthFormat;
} FlVulkanDevice;

typedef struct FlVulkanImage {
    VkImage Handle;
    VkDeviceMemory Memory;
    VkImageView ImageView;
    FlUInt32 Width;
    FlUInt32 Height;
} FlVulkanImage;

typedef enum FlVulkanRenderPassState {
    FlVulkanRenderPassStateReady,
    FlVulkanRenderPassStateRecording,
    FlVulkanRenderPassStateInRenderPass,
    FlVulkanRenderPassStateRecordingEnded,
    FlVulkanRenderPassStateSubmitted,
    FlVulkanRenderPassStateNotAllocated
} FlVulkanRenderPassState;

typedef struct FlVulkanRenderPass {
    VkRenderPass Handle;
    FlFloat32 X, Y, W, H;
    FlFloat32 R, G, B, A;

    FlFloat32 Depth;
    FlUInt32 Stencil;

    FlVulkanRenderPassState State;
} FlVulkanRenderPass;

typedef struct FlVulkanFramebuffer {
    VkFramebuffer Handle;
    FlUInt32 AttachmentCount;
    VkImageView* Attachments;
    FlVulkanRenderPass* RenderPass;
} FlVulkanFramebuffer;

typedef struct FlVulkanSwapchain {
    VkSurfaceFormatKHR ImageFormat;
    FlUInt8 MaxFramesInFlight;

    VkSwapchainKHR Handle;

    FlUInt32 ImageCount;
    VkImage* Images;
    VkImageView* ImageViews;

    FlVulkanImage DepthAttachment;

    FlVulkanFramebuffer* Framebuffers;
} FlVulkanSwapchain;

typedef enum FlVulkanCommandBufferState {
    FlVulkanCommandBufferStateReady,
    FlVulkanCommandBufferStateRecording,
    FlVulkanCommandBufferStateInRenderPass,
    FlVulkanCommandBufferStateRecordingEnded,
    FlVulkanCommandBufferStateSubmitted,
    FlVulkanCommandBufferStateNotAllocated
} FlVulkanCommandBufferState;

typedef struct FlVulkanCommandBuffer {
    VkCommandBuffer Handle;

    FlVulkanCommandBufferState State;
} FlVulkanCommandBuffer;

typedef struct FlVulkanFence {
    VkFence Handle;
    FlBool8 Signaled;
} FlVulkanFence;

typedef struct FlVulkanContext {
    FlUInt32 FramebufferWidth;
    FlUInt32 FramebufferHeight;

    VkInstance Instance;

    VkAllocationCallbacks* Allocator;

    VkSurfaceKHR Surface;

#ifdef FL_DEBUG
    VkDebugUtilsMessengerEXT DebugMessenger;
#endif

    FlVulkanDevice Device;

    FlVulkanSwapchain Swapchain;
    FlVulkanRenderPass MainRenderPass;

    // DArray
    FlVulkanCommandBuffer* GraphicsCommandBuffers;

    // DArray
    VkSemaphore* ImageAvailableSemaphores;
    
    // DArray
    VkSemaphore* QueueCompleteSemaphores;
    
    FlUInt32 InFlightFenceCount;
    FlVulkanFence* InFlightFences;

    // Holds pointer to fences which exist and are owned elsewhere.
    FlVulkanFence** ImagesInFlight;

    FlUInt32 ImageIndex;
    FlUInt32 CurrentFrame;

    FlBool8 RecreatingSwapchain;

    FlInt32 (*FindMemoryIndex)(FlUInt32 typeFilter, FlUInt32 propertyFlags);
} FlVulkanContext;

#endif // FL_RENDERER_VULKAN_VULKANTYPES_INL
