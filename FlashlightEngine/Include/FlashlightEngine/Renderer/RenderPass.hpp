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

namespace Flashlight {
    struct RenderPassInfo {
        std::vector<VkAttachmentDescription> Attachments{};
        std::vector<VkAttachmentReference> AttachmentReferences{};
        std::vector<VkSubpassDescription> Subpasses{};
    };
    
    class RenderPass {
        VkRenderPass m_RenderPass = VK_NULL_HANDLE;

        VkDevice m_Device = VK_NULL_HANDLE;
        
    public:
        inline RenderPass(const VulkanWrapper::Device& device, const RenderPassInfo& renderPassInfo);
        inline ~RenderPass();

        RenderPass(const RenderPass&) = delete;
        RenderPass(RenderPass&&) = delete;

        RenderPass& operator=(const RenderPass&) = delete;
        RenderPass& operator=(RenderPass&&) = delete;

        [[nodiscard]] VkRenderPass GetNativeRenderPass() const;

        static RenderPassInfo UseDefaultRenderPassInfo(VkFormat swapChainImageFormat);

    private:
        void Create(const RenderPassInfo& renderPassInfo);
        inline void Destroy() const;
    };

#include "RenderPass.inl"
}
