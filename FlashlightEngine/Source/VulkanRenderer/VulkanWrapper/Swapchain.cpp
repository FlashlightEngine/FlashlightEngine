/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Swapchain.cpp
 * Description : Definitions of methods from the Swapchain class.
 */
#include <FlashlightEngine/VulkanRenderer/VulkanWrapper/Swapchain.hpp>

namespace Flashlight::VulkanRenderer::VulkanWrapper {
    Swapchain::Swapchain(const std::shared_ptr<Device>& device, const Surface& surface,
                         const Window& window) : m_Device(device) {
        Log::EngineTrace("Creating Vulkan swapchain...");
        vkb::SwapchainBuilder swapchainBuilder{
            m_Device->GetPhysicalDevice(), m_Device->GetLogicalDevice(), surface.GetSurface()
        };

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
    }

    Swapchain::~Swapchain() {
        Log::EngineTrace("Destroying Vulkan swapchain.");
        vkDestroySwapchainKHR(m_Device->GetLogicalDevice(), m_Swapchain, nullptr);

        Log::EngineTrace("Destroying Vulkan swapchain image views.");
        for (const auto swapchainImageView : m_SwapchainImageViews) {
            vkDestroyImageView(m_Device->GetLogicalDevice(), swapchainImageView, nullptr);
        }
    }

    VkResult Swapchain::AcquireNextSwapchainImage(const FrameData& frame, u32& imageIndex) const {
        VK_CHECK(vkWaitForFences(m_Device->GetLogicalDevice(), 1, &frame.RenderFence, true, 1000000000))
        VK_CHECK(vkResetFences(m_Device->GetLogicalDevice(), 1, &frame.RenderFence))

        return vkAcquireNextImageKHR(m_Device->GetLogicalDevice(), m_Swapchain, 1000000000,
                                     frame.SwapchainSemaphore, nullptr, &imageIndex);
    }

    Swapchain::Swapchain(Swapchain&& other) noexcept {
        m_Swapchain = other.m_Swapchain;
        m_SwapchainImageFormat = other.m_SwapchainImageFormat;
        m_SwapchainImages = std::move(other.m_SwapchainImages);
        m_SwapchainImageViews = std::move(other.m_SwapchainImageViews);
        m_SwapchainExtent = other.m_SwapchainExtent;

        m_Device = std::move(other.m_Device);
        
        other.m_Swapchain = VK_NULL_HANDLE;
        other.m_SwapchainImageFormat = VK_FORMAT_UNDEFINED;
        other.m_SwapchainExtent = {0, 0};
    }

    Swapchain& Swapchain::operator=(Swapchain&& other) noexcept {
        m_Swapchain = other.m_Swapchain;
        m_SwapchainImageFormat = other.m_SwapchainImageFormat;
        m_SwapchainImages = std::move(other.m_SwapchainImages);
        m_SwapchainImageViews = std::move(other.m_SwapchainImageViews);
        m_SwapchainExtent = other.m_SwapchainExtent;

        m_Device = std::move(other.m_Device);
        
        other.m_Swapchain = VK_NULL_HANDLE;
        other.m_SwapchainImageFormat = VK_FORMAT_UNDEFINED;
        other.m_SwapchainExtent = {0, 0};

        return *this;
    }
}
