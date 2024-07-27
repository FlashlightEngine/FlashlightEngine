/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Framebuffer.hpp
 * Description : Declaration of the Framebuffer class. A framebuffer is used as a render target for a render pass.
 */
#pragma once

#include "FlashlightEngine/Renderer/VulkanWrapper/Device.hpp"
#include "FlashlightEngine/Renderer/VulkanWrapper/RenderPass.hpp"

namespace Flashlight::VulkanWrapper {
    class Framebuffer {
        VkFramebuffer m_Framebuffer = VK_NULL_HANDLE;

        std::shared_ptr<Device> m_Device;

    public:
        Framebuffer(const std::shared_ptr<Device>& device, const std::vector<VkImageView>& attachedImageViews, const RenderPass& renderPass,
                    VkExtent2D extent);
        inline ~Framebuffer();

        Framebuffer(const Framebuffer&) = delete;
        inline Framebuffer(Framebuffer&& rhs) noexcept;

        Framebuffer& operator=(const Framebuffer&) = delete;
        inline Framebuffer& operator=(Framebuffer&& rhs) noexcept;

        [[nodiscard]] inline VkFramebuffer GetNativeFramebuffer() const;
    };

#include "Framebuffer.inl"
}
