/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : SwapChain.cpp
 * Description : Definitions of methods from the SwapChain class.
 */
#include "FlashlightEngine/Renderer/VulkanWrapper/SwapChain.hpp"

#include <algorithm>
#include <cstdint>
#include <limits>

namespace Flashlight::VulkanWrapper {
    SwapChain::SwapChain(Device& device, const Window& window,
                                const Surface& surface) : m_Device(device),
                                                          m_Surface(surface.GetNativeSurface()),
                                                          m_QueueFamilies(device.GetQueueFamilies()),
                                                          m_SwapChainSupport(device.GetSwapChainSupport()),
                                                          m_Window(window.GetGlfwWindow()) {
        CreateSwapChain();
        CreateSwapChainImageViews();
        CreateRenderPass();
        CreateFramebuffers();
    }
    
    void SwapChain::CreateSwapChain() {
        const VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(m_SwapChainSupport.Formats);
        const VkPresentModeKHR presentMode = ChooseSwapPresentMode(m_SwapChainSupport.PresentModes);
        const VkExtent2D extent = ChooseSwapExtent(m_SwapChainSupport.Capabilities);

        u32 imageCount = m_SwapChainSupport.Capabilities.minImageCount + 1;

        // 0 is a special value for maxImageCount. It means there is no maximum.
        if (m_SwapChainSupport.Capabilities.maxImageCount > 0 &&
            imageCount > m_SwapChainSupport.Capabilities.maxImageCount) {
            imageCount = m_SwapChainSupport.Capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR swapChainCreateInfo{};
        swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapChainCreateInfo.surface = m_Surface;
        swapChainCreateInfo.minImageCount = imageCount;
        swapChainCreateInfo.imageFormat = surfaceFormat.format;
        swapChainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
        swapChainCreateInfo.imageExtent = extent;
        swapChainCreateInfo.imageArrayLayers = 1;
        swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        const u32 queueFamilyIndices[] = {m_QueueFamilies.GraphicsFamily, m_QueueFamilies.PresentFamily};

        if (m_QueueFamilies.GraphicsFamily != m_QueueFamilies.PresentFamily) {
            swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            swapChainCreateInfo.queueFamilyIndexCount = 2;
            swapChainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
        } else {
            swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            swapChainCreateInfo.queueFamilyIndexCount = 0; // Optional
            swapChainCreateInfo.pQueueFamilyIndices = nullptr; // Optional
        }

        swapChainCreateInfo.preTransform = m_SwapChainSupport.Capabilities.currentTransform;

        swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

        swapChainCreateInfo.presentMode = presentMode;
        swapChainCreateInfo.clipped = VK_TRUE;

        swapChainCreateInfo.oldSwapchain = nullptr;

        Log::EngineTrace("Creating Vulkan swap chain...");
        if (vkCreateSwapchainKHR(m_Device.GetNativeDevice(), &swapChainCreateInfo, nullptr, &m_SwapChain) != VK_SUCCESS) {
            Log::EngineError("Failed to create Vulkan swap chain.");
        } else {
            Log::EngineTrace("Vulkan swap chain created.");
        }

        vkGetSwapchainImagesKHR(m_Device.GetNativeDevice(), m_SwapChain, &imageCount, nullptr);
        m_SwapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(m_Device.GetNativeDevice(), m_SwapChain, &imageCount, m_SwapChainImages.data());

        m_SwapChainImageFormat = surfaceFormat.format;
        m_SwapChainExtent = extent;
    }

    void SwapChain::CreateSwapChainImageViews() {
        Log::EngineTrace("Creating Vulkan swap chain image views...");

        m_SwapChainImageViews.resize(m_SwapChainImages.size());

        for (size i = 0; i < m_SwapChainImages.size(); i++) {
            VkImageViewCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = m_SwapChainImages[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = m_SwapChainImageFormat;

            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;

            if (vkCreateImageView(m_Device.GetNativeDevice(), &createInfo, nullptr, &m_SwapChainImageViews[i]) != VK_SUCCESS) {
                Log::EngineError("Failed to create Vulkan swap chain image views.");
            }
        }
    }

    void SwapChain::CreateRenderPass() {
        VkAttachmentDescription colorAttachment{};        
        colorAttachment.format = m_SwapChainImageFormat;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentReference;
        colorAttachmentReference.attachment = 0;
        colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentReference;

        RenderPassDescription description;
        description.Attachments = {colorAttachment};
        description.Subpasses = {subpass};

        m_RenderPass = std::make_unique<RenderPass>(m_Device, description);
    }

    void SwapChain::CreateFramebuffers() {
        m_Framebuffers.reserve(m_SwapChainImageViews.size());
        
        for (size i = 0; i < m_SwapChainImageViews.size(); i++) {
            const std::vector<VkImageView> attachments = {
                m_SwapChainImageViews[i]
            };
            m_Framebuffers.emplace_back(m_Device, attachments, *m_RenderPass, m_SwapChainExtent);      
        }
    }

    VkSurfaceFormatKHR SwapChain::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
        for (const auto& availableFormat : availableFormats) {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace ==
                VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return availableFormat;
            }
        }

        return availableFormats[0];
    }

    VkPresentModeKHR SwapChain::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
        for (const auto& availablePresentMode : availablePresentModes) {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                return availablePresentMode;
            }
        }

        return VK_PRESENT_MODE_FIFO_KHR; // Only VK_PRESENT_MODE_FIFO_KHR is guaranteed to be available. 
    }

    VkExtent2D SwapChain::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) const {
        // The current extent already contains the extent that matches the best the window. However, if the members
        // of the current extent are set to the max value of uint32_t, that means the window manager lets us differ.
        if (capabilities.currentExtent.width != std::numeric_limits<u32>::max()) {
            return capabilities.currentExtent;
        }

        int width, height;
        glfwGetFramebufferSize(m_Window, &width, &height);

        VkExtent2D actualExtent = {
            static_cast<u32>(width),
            static_cast<u32>(height)
        };

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width,
                                        capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height,
                                         capabilities.maxImageExtent.height);

        return actualExtent;
    }
}
