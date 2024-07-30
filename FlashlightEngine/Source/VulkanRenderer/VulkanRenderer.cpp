/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : VulkanRenderer.cpp
 * Description : Definitions of methods from the VulkanRenderer class.
 */
#include <FlashlightEngine/VulkanRenderer/VulkanRenderer.hpp>

#include <FlashlightEngine/VulkanRenderer/VulkanInitializers.hpp>
#include <FlashlightEngine/VulkanRenderer/VulkanUtils/VulkanImageUtils.hpp>

#include <magic_enum.hpp>

#include <SDL_vulkan.h>

namespace Flashlight::VulkanRenderer {
    namespace {
        VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
            const VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            const VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void* pUserData) {
            std::stringstream message;

            // Severity
            if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
                message << "[INFO] ";
            }

            if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) {
                message << "[VERBOSE] ";
            }

            if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
                message << "[WARNING] ";
            }

            if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
                message << "[ERROR] ";
            }

            // Type
            if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT) {
                message << "[GENERAL] ";
            }

            if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) {
                message << "[VALIDATION] ";
            }

            if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT) {
                message << "[PERFORMANCE] ";
            }

            message << '[' << pCallbackData->messageIdNumber << ' ';
            if (pCallbackData->pMessageIdName) {
                message << pCallbackData->pMessageIdName;
            }

            message << "]: " << pCallbackData->pMessage;

            if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT ||
                messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) {
                Log::EngineTrace(message.str());
            } else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
                Log::EngineWarn(message.str());
            } else {
                Log::EngineError(message.str());
            }

            return VK_FALSE;
        }
    }

    VulkanRenderer::VulkanRenderer(const Window& window, const DebugLevel& debugLevel) {
        InitializeVulkan(window, debugLevel);
        InitializeSwapchain(window);
        InitializeCommands();
        InitializeSynchronisationPrimitives();
    }

    VulkanRenderer::~VulkanRenderer() {
        vkDeviceWaitIdle(m_Device);

        for (u32 i = 0; i < g_FrameOverlap; i++) {
            Log::EngineTrace("Destroying Vulkan command pool for frame #{0}.", i);
            vkDestroyCommandPool(m_Device, m_Frames[i].CommandPool, nullptr);

            Log::EngineTrace("Destroying Vulkan synchronisation objects for frame #{0}.", i);
            vkDestroyFence(m_Device, m_Frames[i].RenderFence, nullptr);
            vkDestroySemaphore(m_Device, m_Frames[i].RenderSemaphore, nullptr);
            vkDestroySemaphore(m_Device, m_Frames[i].SwapchainSemaphore, nullptr);
        }

        if (m_Swapchain) {
            DestroySwapchain();
        }

        if (m_Device) {
            Log::EngineTrace("Destroying Vulkan device.");
            vkDestroyDevice(m_Device, nullptr);
        }

        if (m_Surface) {
            Log::EngineTrace("Destroying Vulkan surface.");
            vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
        }

        if (m_DebugMessenger) {
            Log::EngineTrace("Destroying Vulkan debug messenger.");
            vkb::destroy_debug_utils_messenger(m_Instance, m_DebugMessenger);
        }

        if (m_Instance) {
            Log::EngineTrace("Destroying Vulkan instance.");
            vkDestroyInstance(m_Instance, nullptr);
        }
    }

    void VulkanRenderer::BeginFrame() {
        VK_CHECK(vkWaitForFences(m_Device, 1, &GetCurrentFrame().RenderFence, true, 1000000000))
        VK_CHECK(vkResetFences(m_Device, 1, &GetCurrentFrame().RenderFence))

        u32 swapchainImageIndex;
        VK_CHECK(
            vkAcquireNextImageKHR(m_Device, m_Swapchain, 1000000000, GetCurrentFrame().SwapchainSemaphore, nullptr,
                &swapchainImageIndex))

        const VkCommandBuffer cmd = GetCurrentFrame().MainCommandBuffer;

        VK_CHECK(vkResetCommandBuffer(cmd, 0))

        const VkCommandBufferBeginInfo cmdBeginInfo = VulkanInit::CommandBufferBeginInfo(
            VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

        VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo))

        VulkanUtils::TransitionImage(cmd, m_SwapchainImages[swapchainImageIndex], VK_IMAGE_LAYOUT_UNDEFINED,
                                     VK_IMAGE_LAYOUT_GENERAL);

        VkClearColorValue clearValue;
        const float flash = std::abs(std::sin(static_cast<f32>(m_FrameNumber) / 120.0f));
        clearValue = {{0.0f, 0.0f, flash, 1.0f}};

        VkImageSubresourceRange clearRange = VulkanInit::ImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT);

        vkCmdClearColorImage(cmd, m_SwapchainImages[swapchainImageIndex], VK_IMAGE_LAYOUT_GENERAL, &clearValue, 1,
                             &clearRange);

        VulkanUtils::TransitionImage(cmd, m_SwapchainImages[swapchainImageIndex], VK_IMAGE_LAYOUT_GENERAL,
                                     VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

        VK_CHECK(vkEndCommandBuffer(cmd))

        VkCommandBufferSubmitInfo cmdInfo = VulkanInit::CommandBufferSubmitInfo(cmd);

        VkSemaphoreSubmitInfo waitInfo = VulkanInit::SemaphoreSubmitInfo(VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, GetCurrentFrame().SwapchainSemaphore);
        VkSemaphoreSubmitInfo signalInfo = VulkanInit::SemaphoreSubmitInfo(VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, GetCurrentFrame().RenderSemaphore);

        const VkSubmitInfo2 submit = VulkanInit::SubmitInfo(&cmdInfo, &signalInfo, &waitInfo);
        
        VK_CHECK(vkQueueSubmit2(m_GraphicsQueue, 1, &submit, GetCurrentFrame().RenderFence))

        VkPresentInfoKHR presentInfo {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.pNext = nullptr;
        presentInfo.pSwapchains = &m_Swapchain;
        presentInfo.swapchainCount = 1;

        presentInfo.pWaitSemaphores = &GetCurrentFrame().RenderSemaphore;
        presentInfo.waitSemaphoreCount = 1;

        presentInfo.pImageIndices = &swapchainImageIndex;

        VK_CHECK(vkQueuePresentKHR(m_GraphicsQueue, &presentInfo))

        m_FrameNumber++;
    }

    void VulkanRenderer::InitializeVulkan(const Window& window, const DebugLevel& debugLevel) {
        volkInitialize();

        /*
         * Instance & debug messenger creation
         */
        Log::EngineTrace("Creating Vulkan instance & debug messenger...");
        vkb::InstanceBuilder builder;

        u32 extensionCount = 0;
        SDL_Vulkan_GetInstanceExtensions(window.GetNativeWindow(), &extensionCount, nullptr);
        std::vector<const char*> requiredExtensions(extensionCount);
        SDL_Vulkan_GetInstanceExtensions(window.GetNativeWindow(), &extensionCount, requiredExtensions.data());

        Log::EngineTrace("Vulkan instance extensions required by the window:");
        for (const auto extension : requiredExtensions) {
            Log::EngineTrace("\t - {0}", extension);
        }

        builder.set_app_name(window.GetTitle().c_str())
               .set_app_version(1, 0, 0)
               .set_engine_name("Flashlight Engine")
               .set_engine_version(1, 0, 0)
               .request_validation_layers(debugLevel > DebugLevel::None)
               .require_api_version(1, 3, 0);

        VkDebugUtilsMessageSeverityFlagsEXT messageSeverity = 0;

        if (debugLevel >= DebugLevel::Errors) {
            messageSeverity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        }

        if (debugLevel >= DebugLevel::Warnings) {
            messageSeverity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
        }

        if (debugLevel >= DebugLevel::Verbose) {
            messageSeverity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
        }

        if (debugLevel >= DebugLevel::Debug) {
            messageSeverity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;
        }

        constexpr VkDebugUtilsMessageTypeFlagsEXT messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

        builder.add_debug_messenger_severity(messageSeverity)
               .add_debug_messenger_type(messageType)
               .set_debug_callback(DebugCallback);

        auto builderResult = builder.build();

        const vkb::Instance vkbInstance = builderResult.value();

        Log::EngineTrace("Vulkan Instance & debug messenger created.");
        m_Instance = vkbInstance.instance;
        m_DebugMessenger = vkbInstance.debug_messenger;

        volkLoadInstanceOnly(m_Instance);

        /*
         * Window surface creation 
         */
        Log::EngineTrace("Creating Vulkan window surface...");
        SDL_Vulkan_CreateSurface(window.GetNativeWindow(), m_Instance, &m_Surface);
        Log::EngineTrace("Vulkan window surface created.");

        /*
         * Physical device selection & logical device creation.
         */
        // Vulkan 1.3 features
        VkPhysicalDeviceVulkan13Features features = {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES
        };
        features.dynamicRendering = VK_TRUE;
        features.synchronization2 = VK_TRUE;

        // Vulkan 1.2 features
        VkPhysicalDeviceVulkan12Features features12 = {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES
        };
        features12.bufferDeviceAddress = VK_TRUE;
        features12.descriptorIndexing = VK_TRUE;

        Log::EngineTrace("Selecting Vulkan physical device & creating Vulkan logical device...");
        vkb::PhysicalDeviceSelector selector{vkbInstance};
        vkb::PhysicalDevice physicalDevice = selector
                                             .set_minimum_version(1, 3)
                                             .set_required_features_13(features)
                                             .set_required_features_12(features12)
                                             .set_surface(m_Surface)
                                             .select()
                                             .value();

        // Create the final Vulkan device.
        vkb::DeviceBuilder deviceBuilder{physicalDevice};

        vkb::Device vkbDevice = deviceBuilder.build().value();

        Log::EngineTrace("Vulkan physical device selected & logical device created.");
        m_Device = vkbDevice.device;
        m_PhysicalDevice = physicalDevice.physical_device;

        volkLoadDevice(m_Device);

        VkPhysicalDeviceProperties physicalDeviceProperties;
        vkGetPhysicalDeviceProperties(m_PhysicalDevice, &physicalDeviceProperties);

        Log::EngineTrace("Vulkan physical device properties:");
        Log::EngineTrace("\t - Device name:           {0}", physicalDeviceProperties.deviceName);

        Log::EngineTrace("\t - Device type:           {0}",
                         magic_enum::enum_name(physicalDeviceProperties.deviceType));

        Log::EngineTrace("\t - Device API version:    {0}.{1}.{2}.{3}",
                         VK_API_VERSION_VARIANT(physicalDeviceProperties.apiVersion),
                         VK_API_VERSION_MAJOR(physicalDeviceProperties.apiVersion),
                         VK_API_VERSION_MINOR(physicalDeviceProperties.apiVersion),
                         VK_API_VERSION_PATCH(physicalDeviceProperties.apiVersion));

        // 4318 : NVIDIA
        if (physicalDeviceProperties.vendorID == 4318) {
            Log::EngineTrace("\t - Driver version:        {0}.{1}.{2}.{3}",
                             (physicalDeviceProperties.driverVersion >> 22) & 0x3FF,
                             (physicalDeviceProperties.driverVersion >> 14) & 0x0FF,
                             (physicalDeviceProperties.driverVersion >> 6) & 0x0FF,
                             (physicalDeviceProperties.driverVersion) & 0x003F);
        }

#if defined(_WIN32) || defined(_WIN64)
        // 0x8086 : Intel, only differs on Windows.
        else if (physicalDeviceProperties.vendorID == 0x8086) {
            Log::EngineTrace("\t - Driver version:          {0}.{1}",
                             (physicalDeviceProperties.driverVersion >> 14),
                             (physicalDeviceProperties.driverVersion) & 0x3FFF);
        }
#endif
        // For other vendors, use the Vulkan version convention.
        else {
            Log::EngineTrace("\t - Driver version:          {0}.{0}.{0}",
                             VK_API_VERSION_MAJOR(physicalDeviceProperties.driverVersion),
                             VK_API_VERSION_MINOR(physicalDeviceProperties.driverVersion),
                             VK_API_VERSION_PATCH(physicalDeviceProperties.driverVersion));
        }

        m_GraphicsQueue = vkbDevice.get_queue(vkb::QueueType::graphics).value();
        m_GraphicsQueueFamily = vkbDevice.get_queue_index(vkb::QueueType::graphics).value();
    }


    void VulkanRenderer::InitializeSwapchain(const Window& window) {
        CreateSwapchain(window.GetWidth(), window.GetHeight());
    }

    void VulkanRenderer::InitializeCommands() {
        const VkCommandPoolCreateInfo commandPoolInfo = VulkanInit::CommandPoolCreateInfo(
            m_GraphicsQueueFamily, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

        Log::EngineTrace("Creating frames Vulkan command pool & command buffer...");
        for (u32 i = 0; i < g_FrameOverlap; i++) {
            VK_CHECK(vkCreateCommandPool(m_Device, &commandPoolInfo, nullptr, &m_Frames[i].CommandPool))
            Log::EngineTrace("Created Vulkan command pool for frame #{0}", i);

            VkCommandBufferAllocateInfo cmdAllocInfo = VulkanInit::CommandBufferAllocateInfo(
                m_Frames[i].CommandPool);

            VK_CHECK(vkAllocateCommandBuffers(m_Device, &cmdAllocInfo, &m_Frames[i].MainCommandBuffer))
            Log::EngineTrace("Created Vulkan command command for frame #{0}", i);
        }
    }

    void VulkanRenderer::InitializeSynchronisationPrimitives() {
        const VkFenceCreateInfo fenceCreateInfo = VulkanInit::FenceCreateInfo(VK_FENCE_CREATE_SIGNALED_BIT);
        const VkSemaphoreCreateInfo semaphoreCreateInfo = VulkanInit::SemaphoreCreateInfo();

        Log::EngineTrace("Creating Vulkan synchronisation primitives...");
        for (u32 i = 0; i < g_FrameOverlap; i++) {
            VK_CHECK(vkCreateFence(m_Device, &fenceCreateInfo, nullptr, &m_Frames[i].RenderFence))
            Log::EngineTrace("Vulkan render fence created for frame #{0}", i);

            VK_CHECK(vkCreateSemaphore(m_Device, &semaphoreCreateInfo, nullptr, &m_Frames[i].SwapchainSemaphore))
            Log::EngineTrace("Vulkan swapchain semaphore created for frame #{0}", i);
            VK_CHECK(vkCreateSemaphore(m_Device, &semaphoreCreateInfo, nullptr, &m_Frames[i].RenderSemaphore))
            Log::EngineTrace("Vulkan render semaphore created for frame #{0}", i);
        }
    }

    void VulkanRenderer::CreateSwapchain(const u32 width, const u32 height) {
        Log::EngineTrace("Creating Vulkan swapchain...");
        vkb::SwapchainBuilder swapchainBuilder{m_PhysicalDevice, m_Device, m_Surface};

        m_SwapchainImageFormat = VK_FORMAT_B8G8R8A8_UNORM;

        vkb::Swapchain vkbSwapchain = swapchainBuilder
                                      .set_desired_format(VkSurfaceFormatKHR{
                                          .format = m_SwapchainImageFormat,
                                          .colorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR
                                      })
                                      .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
                                      .set_desired_extent(width, height)
                                      .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
                                      .build()
                                      .value();

        Log::EngineTrace("Vulkan swapchain created.");
        m_SwapchainExtent = vkbSwapchain.extent;
        m_Swapchain = vkbSwapchain.swapchain;
        m_SwapchainImages = vkbSwapchain.get_images().value();
        m_SwapchainImageViews = vkbSwapchain.get_image_views().value();
    }

    void VulkanRenderer::DestroySwapchain() const {
        Log::EngineTrace("Destroying Vulkan swapchain.");
        vkDestroySwapchainKHR(m_Device, m_Swapchain, nullptr);

        Log::EngineTrace("Destroying Vulkan swapchain image views.");
        for (const auto swapchainImageView : m_SwapchainImageViews) {
            vkDestroyImageView(m_Device, swapchainImageView, nullptr);
        }
    }
}
