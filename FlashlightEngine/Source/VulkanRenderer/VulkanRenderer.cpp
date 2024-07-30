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

namespace Flashlight::VulkanRenderer {

    VulkanRenderer::VulkanRenderer(const Window& window, const DebugLevel& debugLevel) {
        InitializeVulkan(window, debugLevel);
        InitializeSwapchain(window);
        InitializeCommands();
        InitializeSynchronisationPrimitives();
    }

    VulkanRenderer::~VulkanRenderer() {
        vkDeviceWaitIdle(m_Device->GetLogicalDevice());

        for (u32 i = 0; i < g_FrameOverlap; i++) {
            Log::EngineTrace("Destroying Vulkan command pool for frame #{0}.", i);
            vkDestroyCommandPool(m_Device->GetLogicalDevice(), m_Frames[i].CommandPool, nullptr);

            Log::EngineTrace("Destroying Vulkan synchronisation objects for frame #{0}.", i);
            vkDestroyFence(m_Device->GetLogicalDevice(), m_Frames[i].RenderFence, nullptr);
            vkDestroySemaphore(m_Device->GetLogicalDevice(), m_Frames[i].RenderSemaphore, nullptr);
            vkDestroySemaphore(m_Device->GetLogicalDevice(), m_Frames[i].SwapchainSemaphore, nullptr);
        }
    }

    VkCommandBuffer VulkanRenderer::BeginFrame() {
        auto& frame = GetCurrentFrame();

        u32 swapchainImageIndex;
        VK_CHECK(m_Swapchain->AcquireNextSwapchainImage(frame, swapchainImageIndex))

        frame.SwapchainImageIndex = swapchainImageIndex;

        const VkCommandBuffer cmd = frame.MainCommandBuffer;

        VK_CHECK(vkResetCommandBuffer(cmd, 0))

        const VkCommandBufferBeginInfo cmdBeginInfo = VulkanInit::CommandBufferBeginInfo(
            VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

        VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo))

        VulkanUtils::TransitionImage(cmd, m_Swapchain->GetImageAtIndex(swapchainImageIndex),
                                     VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

        VkClearColorValue clearValue;
        const float flash = std::abs(std::sin(static_cast<f32>(m_FrameNumber) / 120.0f));
        clearValue = {{0.0f, 0.0f, flash, 1.0f}};

        const VkImageSubresourceRange clearRange = VulkanInit::ImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT);

        vkCmdClearColorImage(cmd, m_Swapchain->GetImageAtIndex(swapchainImageIndex), VK_IMAGE_LAYOUT_GENERAL,
                             &clearValue, 1, &clearRange);

        return cmd;
    }

    void VulkanRenderer::EndFrame() {
        const auto& frame = GetCurrentFrame();
        VulkanUtils::TransitionImage(frame.MainCommandBuffer, m_Swapchain->GetImageAtIndex(frame.SwapchainImageIndex), VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

        VK_CHECK(vkEndCommandBuffer(frame.MainCommandBuffer))

        const VkCommandBufferSubmitInfo cmdInfo = VulkanInit::CommandBufferSubmitInfo(frame.MainCommandBuffer);

        const VkSemaphoreSubmitInfo waitInfo = VulkanInit::SemaphoreSubmitInfo(
            VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, frame.SwapchainSemaphore);
        const VkSemaphoreSubmitInfo signalInfo = VulkanInit::SemaphoreSubmitInfo(
            VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, frame.RenderSemaphore);

        const VkSubmitInfo2 submit = VulkanInit::SubmitInfo(&cmdInfo, &signalInfo, &waitInfo);

        VK_CHECK(vkQueueSubmit2(m_Device->GetGraphicsQueue(), 1, &submit, frame.RenderFence))

        const VkSwapchainKHR swapchain = m_Swapchain->GetSwapchain();

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.pNext = nullptr;
        presentInfo.pSwapchains = &swapchain;
        presentInfo.swapchainCount = 1;

        presentInfo.pWaitSemaphores = &frame.RenderSemaphore;
        presentInfo.waitSemaphoreCount = 1;

        presentInfo.pImageIndices = &frame.SwapchainImageIndex;

        VK_CHECK(vkQueuePresentKHR(m_Device->GetGraphicsQueue(), &presentInfo))

        m_FrameNumber++;
    }

    void VulkanRenderer::InitializeVulkan(const Window& window, const DebugLevel& debugLevel) {
        m_Instance = std::make_shared<VulkanWrapper::Instance>(window, debugLevel);
        m_Surface = std::make_unique<VulkanWrapper::Surface>(m_Instance, window);
        m_Device = std::make_shared<VulkanWrapper::Device>(m_Instance, *m_Surface);
    }


    void VulkanRenderer::InitializeSwapchain(const Window& window) {
        m_Swapchain = std::make_unique<VulkanWrapper::Swapchain>(m_Device, *m_Surface, window);
    }

    void VulkanRenderer::InitializeCommands() {
        const VkCommandPoolCreateInfo commandPoolInfo = VulkanInit::CommandPoolCreateInfo(
            m_Device->GetGraphicsQueueFamily(), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

        Log::EngineTrace("Creating frames Vulkan command pool & command buffer...");
        for (u32 i = 0; i < g_FrameOverlap; i++) {
            VK_CHECK(
                vkCreateCommandPool(m_Device->GetLogicalDevice(), &commandPoolInfo, nullptr, &m_Frames[i].
                    CommandPool))
            Log::EngineTrace("Created Vulkan command pool for frame #{0}", i);

            VkCommandBufferAllocateInfo cmdAllocInfo = VulkanInit::CommandBufferAllocateInfo(
                m_Frames[i].CommandPool);

            VK_CHECK(
                vkAllocateCommandBuffers(m_Device->GetLogicalDevice(), &cmdAllocInfo, &m_Frames[i].MainCommandBuffer
                ))
            Log::EngineTrace("Created Vulkan command command for frame #{0}", i);
        }
    }

    void VulkanRenderer::InitializeSynchronisationPrimitives() {
        const VkFenceCreateInfo fenceCreateInfo = VulkanInit::FenceCreateInfo(VK_FENCE_CREATE_SIGNALED_BIT);
        const VkSemaphoreCreateInfo semaphoreCreateInfo = VulkanInit::SemaphoreCreateInfo();

        Log::EngineTrace("Creating Vulkan synchronisation primitives...");
        for (u32 i = 0; i < g_FrameOverlap; i++) {
            VK_CHECK(
                vkCreateFence(m_Device->GetLogicalDevice(), &fenceCreateInfo, nullptr, &m_Frames[i].RenderFence))
            Log::EngineTrace("Vulkan render fence created for frame #{0}", i);

            VK_CHECK(
                vkCreateSemaphore(m_Device->GetLogicalDevice(), &semaphoreCreateInfo, nullptr, &m_Frames[i].
                    SwapchainSemaphore))
            Log::EngineTrace("Vulkan swapchain semaphore created for frame #{0}", i);
            VK_CHECK(
                vkCreateSemaphore(m_Device->GetLogicalDevice(), &semaphoreCreateInfo, nullptr, &m_Frames[i].
                    RenderSemaphore))
            Log::EngineTrace("Vulkan render semaphore created for frame #{0}", i);
        }
    }
}
