/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Framebuffer.hpp
 * Description : Declaration of the Framebuffer class. A framebuffer is used as a render target for a render pass.
 */
#pragma once

#include "FlashlightEngine/Renderer/VulkanWrapper/Device.hpp"
#include "FlashlightEngine/Renderer/VulkanWrapper/SwapChain.hpp"
#include "FlashlightEngine/Renderer/RenderPass.hpp"

namespace Flashlight::VulkanWrapper {
    class Framebuffer {
        VkFramebuffer m_Framebuffer = VK_NULL_HANDLE;

        VkDevice m_Device = VK_NULL_HANDLE;

    public:
        inline Framebuffer(const Device& device, const std::vector<VkImageView>& attachedImageViews,
                           const SwapChain& swapChain,
                           const RenderPass& renderPass);
        inline ~Framebuffer();

        Framebuffer(const Framebuffer&) = delete;
        Framebuffer(Framebuffer&&) = delete;

        Framebuffer& operator=(const Framebuffer&) = delete;
        Framebuffer& operator=(Framebuffer&&) = delete;

        [[nodiscard]] inline VkFramebuffer GetNativeFramebuffer() const;

    private:
        void Create(VkRenderPass renderPass, const std::vector<VkImageView>& attachedImageViews, VkExtent2D extent);
        inline void Destroy() const;
    };

#include "Framebuffer.inl"
}
