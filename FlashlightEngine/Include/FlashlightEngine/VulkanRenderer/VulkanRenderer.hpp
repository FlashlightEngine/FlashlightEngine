/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : VulkanRenderer.hpp
 * Description : Declaration of the VulkanRenderer class. It is used to create, manage and destroy vulkan objects.
 */
#pragma once

#include <FlashlightEngine/VulkanRenderer/VulkanTypes.hpp>
#include <FlashlightEngine/VulkanRenderer/VulkanWrapper/Instance.hpp>
#include <FlashlightEngine/VulkanRenderer/VulkanWrapper/Surface.hpp>
#include <FlashlightEngine/VulkanRenderer/VulkanWrapper/Device.hpp>
#include <FlashlightEngine/VulkanRenderer/VulkanWrapper/Swapchain.hpp>

#include <FlashlightEngine/Core/Window.hpp>

#include <VkBootstrap.h>

namespace Flashlight::VulkanRenderer {

    constexpr u32 g_FrameOverlap = 2;

    class VulkanRenderer {
        std::shared_ptr<VulkanWrapper::Instance> m_Instance;
        std::unique_ptr<VulkanWrapper::Surface> m_Surface;
        std::shared_ptr<VulkanWrapper::Device> m_Device;
        std::unique_ptr<VulkanWrapper::Swapchain> m_Swapchain;

        FrameData m_Frames[g_FrameOverlap];
        i32 m_FrameNumber = 0;

        FrameData& GetCurrentFrame() {
            return m_Frames[m_FrameNumber % g_FrameOverlap];
        }

    public:
        VulkanRenderer(const Window& window, const DebugLevel& debugLevel);
        ~VulkanRenderer();

        VulkanRenderer(const VulkanRenderer&) = delete;
        VulkanRenderer(VulkanRenderer&&) = delete;

        VulkanRenderer& operator=(const VulkanRenderer&) = delete;
        VulkanRenderer& operator=(VulkanRenderer&&) = delete;

        VkCommandBuffer BeginFrame();
        void EndFrame();

    private:
        void InitializeVulkan(const Window& window, const DebugLevel& debugLevel);
        void InitializeSwapchain(const Window& window);
        void InitializeCommands();
        void InitializeSynchronisationPrimitives();
    };

}
