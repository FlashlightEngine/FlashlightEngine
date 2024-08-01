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


#include <SDL.h>
#include <SDL_vulkan.h>

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

#include <imgui.h>
#include <imgui_impl_vulkan.h>
#include <imgui_impl_sdl2.h>

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
        InitializeDescriptors();
        InitializePipelines();
        InitializeImGui(window);

        m_RendererInitialized = true;
    }

    VulkanRenderer::~VulkanRenderer() {
        vkDeviceWaitIdle(m_Device);

        if (m_RendererInitialized) {
            m_MainDeletionQueue.Flush();

            m_RendererInitialized = false;
        }
    }

    void VulkanRenderer::CreateUi() {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplSDL2_NewFrame();

        ImGui::NewFrame();

        if (ImGui::Begin("Background effect data")) {
            ComputeEffect& selected = m_BackgroundEffects[m_CurrentBackgroundEffect];

            ImGui::Text("Selected effect: %s", selected.Name);

            ImGui::SliderInt("Effect Index", &m_CurrentBackgroundEffect, 0,
                             static_cast<i32>(m_BackgroundEffects.size() - 1));

            ImGui::InputFloat4("data1", reinterpret_cast<float*>(&selected.Data.Data1));
            ImGui::InputFloat4("data2", reinterpret_cast<float*>(&selected.Data.Data2));
            ImGui::InputFloat4("data3", reinterpret_cast<float*>(&selected.Data.Data3));
            ImGui::InputFloat4("data4", reinterpret_cast<float*>(&selected.Data.Data4));
        }
        ImGui::End();

        ImGui::Render();
    }


    void VulkanRenderer::Draw() {
        auto& frame = GetCurrentFrame();

        u32 swapchainImageIndex;
        VK_CHECK(vkWaitForFences(m_Device, 1, &frame.RenderFence, true, 1000000000))

        frame.DeletionQueue.Flush();

        VK_CHECK(vkResetFences(m_Device, 1, &frame.RenderFence))

        VK_CHECK(
            vkAcquireNextImageKHR(m_Device, m_Swapchain, 1000000000, frame.SwapchainSemaphore, nullptr,
                &swapchainImageIndex))

        frame.SwapchainImageIndex = swapchainImageIndex;

        const VkCommandBuffer cmd = frame.MainCommandBuffer;

        VK_CHECK(vkResetCommandBuffer(cmd, 0))

        const VkCommandBufferBeginInfo cmdBeginInfo = VulkanInit::CommandBufferBeginInfo(
            VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

        m_DrawExtent.width = m_DrawImage.ImageExtent.width;
        m_DrawExtent.height = m_DrawImage.ImageExtent.height;

        VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo))

        VulkanUtils::TransitionImage(cmd, m_DrawImage.Image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

        DrawBackground(cmd);

        // Transition the draw image and the swap chain image into their correct transfer layouts.
        VulkanUtils::TransitionImage(frame.MainCommandBuffer, m_DrawImage.Image, VK_IMAGE_LAYOUT_GENERAL,
                                     VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

        DrawGeometry(cmd);

        VulkanUtils::TransitionImage(frame.MainCommandBuffer, m_DrawImage.Image,
                                     VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                     VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

        VulkanUtils::TransitionImage(frame.MainCommandBuffer, m_SwapchainImages[frame.SwapchainImageIndex],
                                     VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

        VulkanUtils::CopyImageToImage(frame.MainCommandBuffer, m_DrawImage.Image,
                                      m_SwapchainImages[frame.SwapchainImageIndex], m_DrawExtent,
                                      m_SwapchainExtent);

        VulkanUtils::TransitionImage(frame.MainCommandBuffer, m_SwapchainImages[frame.SwapchainImageIndex],
                                     VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                     VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

        DrawImGui(cmd, m_SwapchainImageViews[frame.SwapchainImageIndex]);

        VulkanUtils::TransitionImage(frame.MainCommandBuffer, m_SwapchainImages[frame.SwapchainImageIndex],
                                     VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

        VK_CHECK(vkEndCommandBuffer(frame.MainCommandBuffer))

        const VkCommandBufferSubmitInfo cmdInfo = VulkanInit::CommandBufferSubmitInfo(frame.MainCommandBuffer);

        const VkSemaphoreSubmitInfo waitInfo = VulkanInit::SemaphoreSubmitInfo(
            VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, frame.SwapchainSemaphore);
        const VkSemaphoreSubmitInfo signalInfo = VulkanInit::SemaphoreSubmitInfo(
            VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, frame.RenderSemaphore);

        const VkSubmitInfo2 submit = VulkanInit::SubmitInfo(&cmdInfo, &signalInfo, &waitInfo);

        VK_CHECK(vkQueueSubmit2(m_GraphicsQueue, 1, &submit, frame.RenderFence))

        const VkSwapchainKHR swapchain = m_Swapchain;

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.pNext = nullptr;
        presentInfo.pSwapchains = &swapchain;
        presentInfo.swapchainCount = 1;

        presentInfo.pWaitSemaphores = &frame.RenderSemaphore;
        presentInfo.waitSemaphoreCount = 1;

        presentInfo.pImageIndices = &frame.SwapchainImageIndex;

        VK_CHECK(vkQueuePresentKHR(m_PresentQueue, &presentInfo))

        m_FrameNumber++;
    }

    VkInstance VulkanRenderer::GetVulkanInstance() const {
        return m_Instance;
    }

    VkDevice VulkanRenderer::GetVulkanDevice() const {
        return m_Device;
    }

    void VulkanRenderer::InitializeVulkan(const Window& window, const DebugLevel& debugLevel) {
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

        m_MainDeletionQueue.PushFunction([this]() {
            Log::EngineTrace("Destroying Vulkan debug messenger.");
            vkb::destroy_debug_utils_messenger(m_Instance, m_DebugMessenger, nullptr);

            Log::EngineTrace("Destroying Vulkan instance.");
            vkDestroyInstance(m_Instance, nullptr);
        });

        Log::EngineTrace("Creating Vulkan window surface...");
        SDL_Vulkan_CreateSurface(window.GetNativeWindow(), m_Instance, &m_Surface);
        Log::EngineTrace("Vulkan window surface created.");

        m_MainDeletionQueue.PushFunction([this]() {
            Log::EngineTrace("Destroying Vulkan surface.");
            vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
        });

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
        m_PresentQueue = vkbDevice.get_queue(vkb::QueueType::present).value();
        m_PresentQueueFamily = vkbDevice.get_queue_index(vkb::QueueType::present).value();

        m_MainDeletionQueue.PushFunction([this]() {
            Log::EngineTrace("Destroying Vulkan device.");
            vkDestroyDevice(m_Device, nullptr);
        });

        Log::EngineTrace("Creating VMA allocator.");

        VmaVulkanFunctions functions{};
        functions.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
        functions.vkGetDeviceProcAddr = vkGetDeviceProcAddr;

        VmaAllocatorCreateInfo allocatorInfo{};
        allocatorInfo.physicalDevice = m_PhysicalDevice;
        allocatorInfo.device = m_Device;
        allocatorInfo.instance = m_Instance;
        allocatorInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
        allocatorInfo.pVulkanFunctions = &functions;
        vmaCreateAllocator(&allocatorInfo, &m_Allocator);

        m_MainDeletionQueue.PushFunction([&]() {
            vmaDestroyAllocator(m_Allocator);
        });
    }


    void VulkanRenderer::InitializeSwapchain(const Window& window) {
        Log::EngineTrace("Creating Vulkan swapchain...");
        vkb::SwapchainBuilder swapchainBuilder{m_PhysicalDevice, m_Device, m_Surface};

        m_SwapchainImageFormat = VK_FORMAT_B8G8R8A8_UNORM;

        vkb::Swapchain vkbSwapchain = swapchainBuilder
                                      .set_desired_format(VkSurfaceFormatKHR{
                                          .format = m_SwapchainImageFormat,
                                          .colorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR
                                      })
                                      .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
                                      .set_desired_extent(window.GetWidth(), window.GetHeight())
                                      .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
                                      .build()
                                      .value();

        Log::EngineTrace("Vulkan swapchain created.");
        m_SwapchainExtent = vkbSwapchain.extent;
        m_Swapchain = vkbSwapchain.swapchain;
        m_SwapchainImages = vkbSwapchain.get_images().value();
        m_SwapchainImageViews = vkbSwapchain.get_image_views().value();

        m_MainDeletionQueue.PushFunction([this]() {
            Log::EngineTrace("Destroying Vulkan swapchain.");
            vkDestroySwapchainKHR(m_Device, m_Swapchain, nullptr);

            Log::EngineTrace("Destroying Vulkan swapchain image views.");
            for (const auto swapchainImageView : m_SwapchainImageViews) {
                vkDestroyImageView(m_Device, swapchainImageView, nullptr);
            }
        });

        // Draw image size will match the window.
        const VkExtent3D drawImageExtent = {
            static_cast<u32>(window.GetWidth()), static_cast<u32>(window.GetHeight()), 1
        };

        // Hardcoding the draw format to a 32-bit float.
        m_DrawImage.ImageFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
        m_DrawImage.ImageExtent = drawImageExtent;

        VkImageUsageFlags drawImageUsages{};
        drawImageUsages |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        drawImageUsages |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        drawImageUsages |= VK_IMAGE_USAGE_STORAGE_BIT;
        drawImageUsages |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        const VkImageCreateInfo drawImageInfo = VulkanInit::ImageCreateInfo(
            m_DrawImage.ImageFormat, drawImageUsages, drawImageExtent);

        // For the draw image, we want to allocate it from GPU local memory.
        VmaAllocationCreateInfo drawImageAllocInfo{};
        drawImageAllocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
        drawImageAllocInfo.requiredFlags = static_cast<VkMemoryPropertyFlags>(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        // Allocate and create the image.
        vmaCreateImage(m_Allocator, &drawImageInfo, &drawImageAllocInfo, &m_DrawImage.Image,
                       &m_DrawImage.Allocation, nullptr);

        // Build an image view for the draw image to use for rendering.
        VkImageViewCreateInfo drawImageViewInfo = VulkanInit::ImageViewCreateInfo(
            m_DrawImage.ImageFormat, m_DrawImage.Image, VK_IMAGE_ASPECT_COLOR_BIT);

        VK_CHECK(vkCreateImageView(m_Device, &drawImageViewInfo, nullptr, &m_DrawImage.ImageView))

        // Add to deletion queue.
        m_MainDeletionQueue.PushFunction([this]() {
            vkDestroyImageView(m_Device, m_DrawImage.ImageView, nullptr);
            vmaDestroyImage(m_Allocator, m_DrawImage.Image, m_DrawImage.Allocation);
        });
    }

    void VulkanRenderer::InitializeCommands() {
        VkCommandPoolCreateInfo commandPoolInfo = VulkanInit::CommandPoolCreateInfo(
            m_GraphicsQueueFamily, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

        Log::EngineTrace("Creating frames Vulkan command pool & command buffer...");
        for (u32 i = 0; i < g_FrameOverlap; i++) {
            VK_CHECK(
                vkCreateCommandPool(m_Device, &commandPoolInfo, nullptr, &m_Frames[i].
                    CommandPool))
            Log::EngineTrace("Created Vulkan command pool for frame #{0}", i);

            VkCommandBufferAllocateInfo cmdAllocInfo = VulkanInit::CommandBufferAllocateInfo(
                m_Frames[i].CommandPool);

            VK_CHECK(
                vkAllocateCommandBuffers(m_Device, &cmdAllocInfo, &m_Frames[i].MainCommandBuffer
                ))
            Log::EngineTrace("Created Vulkan command command for frame #{0}", i);
        }

        m_MainDeletionQueue.PushFunction([this]() {
            for (u32 i = 0; i < g_FrameOverlap; i++) {
                Log::EngineTrace("Destroying Vulkan command pool for frame #{0}.", i);
                vkDestroyCommandPool(m_Device, m_Frames[i].CommandPool, nullptr);
            }
        });

        commandPoolInfo = VulkanInit::CommandPoolCreateInfo(m_GraphicsQueueFamily,
                                                            VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT |
                                                            VK_COMMAND_POOL_CREATE_TRANSIENT_BIT);

        Log::EngineTrace("Creating Vulkan command pool for immediate command buffers...");
        VK_CHECK(vkCreateCommandPool(m_Device, &commandPoolInfo, nullptr, &m_ImmediateCommandPool))
        Log::EngineTrace("Vulkan command pool for immediate command buffers created.");

        // Allocate command buffers for immediate submits.
        const VkCommandBufferAllocateInfo cmdAllocInfo = VulkanInit::CommandBufferAllocateInfo(
            m_ImmediateCommandPool);

        Log::EngineTrace("Creating Vulkan immediate command buffer...");
        VK_CHECK(vkAllocateCommandBuffers(m_Device, &cmdAllocInfo, &m_ImmediateCommandBuffer))
        Log::EngineTrace("Vulkan immediate command buffer created.");

        m_MainDeletionQueue.PushFunction([this]() {
            vkDestroyCommandPool(m_Device, m_ImmediateCommandPool, nullptr);
        });
    }

    void VulkanRenderer::InitializeSynchronisationPrimitives() {
        const VkFenceCreateInfo fenceCreateInfo = VulkanInit::FenceCreateInfo(VK_FENCE_CREATE_SIGNALED_BIT);
        const VkSemaphoreCreateInfo semaphoreCreateInfo = VulkanInit::SemaphoreCreateInfo();

        Log::EngineTrace("Creating Vulkan synchronisation primitives...");
        for (u32 i = 0; i < g_FrameOverlap; i++) {
            VK_CHECK(
                vkCreateFence(m_Device, &fenceCreateInfo, nullptr, &m_Frames[i].RenderFence))
            Log::EngineTrace("Vulkan render fence created for frame #{0}", i);

            VK_CHECK(
                vkCreateSemaphore(m_Device, &semaphoreCreateInfo, nullptr, &m_Frames[i].
                    SwapchainSemaphore))
            Log::EngineTrace("Vulkan swapchain semaphore created for frame #{0}", i);
            VK_CHECK(
                vkCreateSemaphore(m_Device, &semaphoreCreateInfo, nullptr, &m_Frames[i].
                    RenderSemaphore))
            Log::EngineTrace("Vulkan render semaphore created for frame #{0}", i);
        }

        m_MainDeletionQueue.PushFunction([this]() {
            for (u32 i = 0; i < g_FrameOverlap; i++) {
                Log::EngineTrace("Destroying Vulkan synchronisation objects for frame #{0}.", i);
                vkDestroyFence(m_Device, m_Frames[i].RenderFence, nullptr);
                vkDestroySemaphore(m_Device, m_Frames[i].RenderSemaphore, nullptr);
                vkDestroySemaphore(m_Device, m_Frames[i].SwapchainSemaphore, nullptr);

                m_Frames[i].DeletionQueue.Flush();
            }
        });

        Log::EngineTrace("Creating Vulkan fence for immediate commands...");
        VK_CHECK(vkCreateFence(m_Device, &fenceCreateInfo, nullptr, &m_ImmediateFence))
        Log::EngineTrace("Vulkan fence for immediate commands created.");

        m_MainDeletionQueue.PushFunction([this]() {
            vkDestroyFence(m_Device, m_ImmediateFence, nullptr);
        });
    }

    void VulkanRenderer::InitializeDescriptors() {
        // Create a descriptor pool that will hold 10 sets with 1 image each.
        std::vector<DescriptorAllocator::PoolSizeRatio> sizes = {
            {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1}
        };

        m_GlobalDescriptorAllocator.InitPool(m_Device, 10, sizes);

        // Make the descriptor set layout for the compute draw.
        {
            DescriptorLayoutBuilder builder;
            builder.AddBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
            m_DrawImageDescriptorLayout = builder.Build(m_Device, VK_SHADER_STAGE_COMPUTE_BIT);
        }

        // Allocate a descriptor set for the compute draw.
        m_DrawImageDescriptors = m_GlobalDescriptorAllocator.Allocate(m_Device, m_DrawImageDescriptorLayout);

        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
        imageInfo.imageView = m_DrawImage.ImageView;

        VkWriteDescriptorSet drawImageWrite{};
        drawImageWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        drawImageWrite.pNext = nullptr;

        drawImageWrite.dstBinding = 0;
        drawImageWrite.dstSet = m_DrawImageDescriptors;
        drawImageWrite.descriptorCount = 1;
        drawImageWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        drawImageWrite.pImageInfo = &imageInfo;

        vkUpdateDescriptorSets(m_Device, 1, &drawImageWrite, 0, nullptr);

        // Make sure both the descriptor allocator and the new layout get cleaned up properly.
        m_MainDeletionQueue.PushFunction([&]() {
            m_GlobalDescriptorAllocator.DestroyPool(m_Device);

            vkDestroyDescriptorSetLayout(m_Device, m_DrawImageDescriptorLayout, nullptr);
        });
    }

    void VulkanRenderer::InitializePipelines() {
        InitializeComputePipelines();
        InitializeTrianglePipeline();
    }

    void VulkanRenderer::InitializeComputePipelines() {
        // Create pipeline layout
        VkPipelineLayoutCreateInfo computeLayout{};
        computeLayout.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        computeLayout.pNext = nullptr;

        computeLayout.setLayoutCount = 1;
        computeLayout.pSetLayouts = &m_DrawImageDescriptorLayout;

        VkPushConstantRange pushConstant;
        pushConstant.offset = 0;
        pushConstant.size = sizeof(ComputePushConstants);
        pushConstant.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

        computeLayout.pushConstantRangeCount = 1;
        computeLayout.pPushConstantRanges = &pushConstant;

        VK_CHECK(vkCreatePipelineLayout(m_Device, &computeLayout, nullptr, &m_ComputePipelineLayout))

        // Create shader modules.
        VkShaderModule gradientShaderModule;
        CreateShaderModule(m_Device, "Shaders/gradient_color.comp", VulkanUtils::ShaderType::Compute,
                           &gradientShaderModule);
        if (!gradientShaderModule) {
            Log::EngineFatal({0x02, 0x00}, "Failed to build the gradient compute shader module.");
        }

        VkShaderModule skyShaderModule;
        CreateShaderModule(m_Device, "Shaders/sky.comp", VulkanUtils::ShaderType::Compute,
                           &skyShaderModule);
        if (!skyShaderModule) {
            Log::EngineFatal({0x02, 0x01}, "Failed to build the sky compute shader module.");
        }

        VkShaderModule gridShaderModule;
        CreateShaderModule(m_Device, "Shaders/gradient_grid.comp", VulkanUtils::ShaderType::Compute,
                           &gridShaderModule);
        if (!skyShaderModule) {
            Log::EngineFatal({0x02, 0x02}, "Failed to build the grid compute shader module.");
        }

        VkPipelineShaderStageCreateInfo stageInfo{};
        stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        stageInfo.pNext = nullptr;
        stageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
        stageInfo.module = gradientShaderModule;
        stageInfo.pName = "main";


        VkComputePipelineCreateInfo computePipelineCreateInfo{};
        computePipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
        computePipelineCreateInfo.pNext = nullptr;
        computePipelineCreateInfo.layout = m_ComputePipelineLayout;
        computePipelineCreateInfo.stage = stageInfo;

        ComputeEffect gradient;
        gradient.Layout = m_ComputePipelineLayout;
        gradient.Name = "Gradient";
        gradient.Data = {};

        // default colors
        gradient.Data.Data1 = glm::vec4(1, 0, 0, 1);
        gradient.Data.Data2 = glm::vec4(0, 0, 1, 1);

        VK_CHECK(
            vkCreateComputePipelines(m_Device, VK_NULL_HANDLE, 1, &computePipelineCreateInfo, nullptr,
                &gradient.Pipeline))

        computePipelineCreateInfo.stage.module = skyShaderModule;

        ComputeEffect sky;
        sky.Layout = m_ComputePipelineLayout;
        sky.Name = "Sky";
        sky.Data = {};

        // Default sky parameters.
        sky.Data.Data1 = glm::vec4(0.1, 0.2, 0.4, 0.97);

        VK_CHECK(
            vkCreateComputePipelines(m_Device, VK_NULL_HANDLE, 1, &computePipelineCreateInfo, nullptr, &sky.Pipeline
            ))

        computePipelineCreateInfo.stage.module = gridShaderModule;

        ComputeEffect grid;
        grid.Layout = m_ComputePipelineLayout;
        grid.Name = "Grid";
        grid.Data = {};

        VK_CHECK(
            vkCreateComputePipelines(m_Device, VK_NULL_HANDLE, 1, &computePipelineCreateInfo, nullptr, &grid.
                Pipeline
            ))

        // Add the 2 effects into the array.
        m_BackgroundEffects.push_back(gradient);
        m_BackgroundEffects.push_back(sky);
        m_BackgroundEffects.push_back(grid);

        vkDestroyShaderModule(m_Device, gradientShaderModule, nullptr);
        vkDestroyShaderModule(m_Device, skyShaderModule, nullptr);
        vkDestroyShaderModule(m_Device, gridShaderModule, nullptr);

        m_MainDeletionQueue.PushFunction([this, sky, gradient, grid]() {
            vkDestroyPipelineLayout(m_Device, m_ComputePipelineLayout, nullptr);
            vkDestroyPipeline(m_Device, sky.Pipeline, nullptr);
            vkDestroyPipeline(m_Device, gradient.Pipeline, nullptr);
            vkDestroyPipeline(m_Device, grid.Pipeline, nullptr);
        });
    }

    void VulkanRenderer::InitializeTrianglePipeline() {
        VkShaderModule triangleVertexShader;
        VulkanUtils::CreateShaderModule(m_Device, "Shaders/colored_triangle.vert", VulkanUtils::ShaderType::Vertex,
                                        &triangleVertexShader);
        if (!triangleVertexShader) {
            Log::EngineFatal({0x02, 0x03}, "Failed to create triangle vertex shader.");
        }

        VkShaderModule triangleFragmentShader;
        VulkanUtils::CreateShaderModule(m_Device, "Shaders/colored_triangle.frag",
                                        VulkanUtils::ShaderType::Fragment, &triangleFragmentShader);
        if (!triangleFragmentShader) {
            Log::EngineFatal({0x02, 0x03}, "Failed to create triangle fragment shader.");
        }

        const VkPipelineLayoutCreateInfo pipelineLayoutInfo = VulkanInit::PipelineLayoutCreateInfo();
        VK_CHECK(vkCreatePipelineLayout(m_Device, &pipelineLayoutInfo, nullptr, &m_TrianglePipelineLayout))

        VulkanUtils::PipelineBuilder pipelineBuilder{};

        pipelineBuilder.SetPipelineLayout(m_TrianglePipelineLayout);
        pipelineBuilder.SetShaders(triangleVertexShader, triangleFragmentShader);
        pipelineBuilder.SetInputTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
        pipelineBuilder.SetPolygonMode(VK_POLYGON_MODE_FILL);
        pipelineBuilder.SetCullMode(VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE);
        pipelineBuilder.SetMultisamplingNone();
        pipelineBuilder.DisableBlending();
        pipelineBuilder.DisableDepthTest();

        pipelineBuilder.SetColorAttachmentFormat(m_DrawImage.ImageFormat);
        pipelineBuilder.SetDepthFormat(VK_FORMAT_UNDEFINED);

        m_TrianglePipeline = pipelineBuilder.BuildPipeline(m_Device);

        vkDestroyShaderModule(m_Device, triangleVertexShader, nullptr);
        vkDestroyShaderModule(m_Device, triangleFragmentShader, nullptr);

        m_MainDeletionQueue.PushFunction([&]() {
            vkDestroyPipelineLayout(m_Device, m_TrianglePipelineLayout, nullptr);
            vkDestroyPipeline(m_Device, m_TrianglePipeline, nullptr);
        });
    }

    void VulkanRenderer::InitializeImGui(const Window& window) {
        // 1: Create descriptor pool for ImGui
        //    The pool is very oversize, but it's copied from ImGui demo
        //    itself.
        const VkDescriptorPoolSize poolSizes[] = {
            {VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
            {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
            {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
            {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
            {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
            {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
            {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
            {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
            {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
            {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
            {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}
        };

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        poolInfo.maxSets = 1000;
        poolInfo.poolSizeCount = static_cast<u32>(std::size(poolSizes));
        poolInfo.pPoolSizes = poolSizes;

        VkDescriptorPool imguiPool;
        VK_CHECK(vkCreateDescriptorPool(m_Device, &poolInfo, nullptr, &imguiPool))

        // 2: Initialize ImGui library.

        ImGui::CreateContext();

        ImGui_ImplSDL2_InitForVulkan(window.GetNativeWindow());

        ImGui_ImplVulkan_InitInfo initInfo = {};
        initInfo.Instance = m_Instance;
        initInfo.PhysicalDevice = m_PhysicalDevice;
        initInfo.Device = m_Device;
        initInfo.QueueFamily = m_GraphicsQueueFamily;
        initInfo.Queue = m_GraphicsQueue;
        initInfo.DescriptorPool = imguiPool;
        initInfo.MinImageCount = 3;
        initInfo.ImageCount = 3;
        initInfo.UseDynamicRendering = true;

        // Dynamic rendering parameters for ImGui to use.
        initInfo.PipelineRenderingCreateInfo = {.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO};
        initInfo.PipelineRenderingCreateInfo.colorAttachmentCount = 1;
        initInfo.PipelineRenderingCreateInfo.pColorAttachmentFormats = &m_SwapchainImageFormat;

        initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

        ImGui_ImplVulkan_LoadFunctions([](const char* functionName, void* vulkanInstance) {
            return vkGetInstanceProcAddr(*(static_cast<VkInstance*>(vulkanInstance)), functionName);
        }, &m_Instance);

        ImGui_ImplVulkan_Init(&initInfo);

        ImGui_ImplVulkan_CreateFontsTexture();

        m_MainDeletionQueue.PushFunction([this, imguiPool]() {
            ImGui_ImplVulkan_Shutdown();
            vkDestroyDescriptorPool(m_Device, imguiPool, nullptr);
        });
    }

    void VulkanRenderer::DrawBackground(const VkCommandBuffer commandBuffer) const {
        const ComputeEffect& effect = m_BackgroundEffects[m_CurrentBackgroundEffect];

        // Bind the gradient drawing compute pipeline.
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, effect.Pipeline);

        // Bind the descriptor set containing the draw image for the compute pipeline.
        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, m_ComputePipelineLayout, 0, 1,
                                &m_DrawImageDescriptors, 0, nullptr);

        vkCmdPushConstants(commandBuffer, m_ComputePipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0,
                           sizeof(ComputePushConstants), &effect.Data);

        // Execute the compute pipeline dispatch. The workgroup size is 16x16, so we need to divide by it.
        vkCmdDispatch(commandBuffer, static_cast<u32>(std::ceil(m_DrawExtent.width / 16.0)),
                      static_cast<u32>(std::ceil(m_DrawExtent.height / 16.0)), 1);
    }

    void VulkanRenderer::DrawGeometry(const VkCommandBuffer commandBuffer) const {
        // Begin a render pass connected to the draw image.
        const VkRenderingAttachmentInfo colorAttachment = VulkanInit::AttachmentInfo(
            m_DrawImage.ImageView, nullptr, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

        const VkRenderingInfo renderingInfo = VulkanInit::RenderingInfo(m_DrawExtent, &colorAttachment, nullptr);
        vkCmdBeginRendering(commandBuffer, &renderingInfo);

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_TrianglePipeline);

        VkViewport viewport;
        viewport.x = 0;
        viewport.y = 0;
        viewport.width = m_DrawExtent.width;
        viewport.height = m_DrawExtent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

        VkRect2D scissor;
        scissor.offset.x = 0;
        scissor.offset.y = 0;
        scissor.extent.width = m_DrawExtent.width;
        scissor.extent.height = m_DrawExtent.height;

        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

        vkCmdDraw(commandBuffer, 3, 1, 0, 0);

        vkCmdEndRendering(commandBuffer);
    }

    void VulkanRenderer::DrawImGui(const VkCommandBuffer commandBuffer, const VkImageView targetImageView) const {
        const VkRenderingAttachmentInfo colorAttachment = VulkanInit::AttachmentInfo(targetImageView, nullptr,
                                                                                     VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
        const VkRenderingInfo renderInfo = VulkanInit::RenderingInfo(m_SwapchainExtent, &colorAttachment, nullptr);

        vkCmdBeginRendering(commandBuffer, &renderInfo);

        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);

        vkCmdEndRendering(commandBuffer);
    }

    void VulkanRenderer::ImmediateSubmit(const std::function<void(VkCommandBuffer commandBuffer)>& function) const {
        VK_CHECK(vkResetFences(m_Device, 1, &m_ImmediateFence))
        VK_CHECK(vkResetCommandBuffer(m_ImmediateCommandBuffer, 0))

        const VkCommandBuffer commandBuffer = m_ImmediateCommandBuffer;

        const VkCommandBufferBeginInfo cmdBeginInfo = VulkanInit::CommandBufferBeginInfo(
            VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

        VK_CHECK(vkBeginCommandBuffer(commandBuffer, &cmdBeginInfo))

        function(commandBuffer);

        VK_CHECK(vkEndCommandBuffer(commandBuffer))

        const VkCommandBufferSubmitInfo cmdInfo = VulkanInit::CommandBufferSubmitInfo(commandBuffer);
        const VkSubmitInfo2 submit = VulkanInit::SubmitInfo(&cmdInfo, nullptr, nullptr);

        // Submit command buffer to the queue and execute it.
        // m_RenderFence will now block until the graphics command finish executing.
        VK_CHECK(vkQueueSubmit2(m_GraphicsQueue, 1, &submit, m_ImmediateFence))

        VK_CHECK(vkWaitForFences(m_Device, 1, &m_ImmediateFence, true, 9999999999))
    }
}
