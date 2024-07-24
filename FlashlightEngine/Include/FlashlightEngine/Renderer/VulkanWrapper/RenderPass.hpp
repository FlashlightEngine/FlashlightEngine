/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : RenderPass.hpp
 * Description : Declaration of the RenderPass class. The render pass wraps framebuffer attachments used during
 *               rendering, how many color and depth attachments there will be, how many samples are used for them
 *               and how their content should be handled.
 */
#pragma once

#include "FlashlightEngine/Renderer/VulkanWrapper/Device.hpp"

namespace Flashlight::VulkanWrapper {
    struct RenderPassDescription {
        std::vector<VkAttachmentDescription> Attachments;
        std::vector<VkSubpassDescription> Subpasses;
    };
    
    class RenderPass {
        VkRenderPass m_RenderPass = VK_NULL_HANDLE;

        Device& m_Device;

    public:
        RenderPass(Device& device, const RenderPassDescription& description);
        inline ~RenderPass();

        RenderPass(const RenderPass&) = delete;
        RenderPass(RenderPass&&) = delete;

        RenderPass& operator=(const RenderPass&) = delete;
        RenderPass& operator=(RenderPass&&) = delete;

        [[nodiscard]] inline VkRenderPass GetNativeRenderPass() const;
    };

#include "RenderPass.inl"
}
