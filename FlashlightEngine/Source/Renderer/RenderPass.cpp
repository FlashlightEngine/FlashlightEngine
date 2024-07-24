/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : RenderPass.cpp
 * Description : Definitions of methods from the RenderPass class.
 */
#include "FlashlightEngine/Renderer/RenderPass.hpp"

namespace Flashlight {
    void RenderPass::Create(const RenderPassInfo& renderPassInfo) {
        VkRenderPassCreateInfo renderPassCreateInfo{};
        renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassCreateInfo.attachmentCount = static_cast<u32>(renderPassInfo.Attachments.size());
        renderPassCreateInfo.pAttachments = renderPassInfo.Attachments.data();
        renderPassCreateInfo.subpassCount = static_cast<u32>(renderPassInfo.Subpasses.size());
        renderPassCreateInfo.pSubpasses = renderPassInfo.Subpasses.data();

        if (vkCreateRenderPass(m_Device, &renderPassCreateInfo, nullptr, &m_RenderPass) != VK_SUCCESS) {
            Log::EngineError("Failed to create render pass.");
        }
    }

    RenderPassInfo RenderPass::UseDefaultRenderPassInfo(const VkFormat swapChainImageFormat) {
        RenderPassInfo renderPassInfo;

        renderPassInfo.Attachments.resize(1);
        
        renderPassInfo.Attachments[0].format = swapChainImageFormat;
        renderPassInfo.Attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
        renderPassInfo.Attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        renderPassInfo.Attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        renderPassInfo.Attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        renderPassInfo.Attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        renderPassInfo.Attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        renderPassInfo.Attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        renderPassInfo.AttachmentReferences.resize(1);
        
        renderPassInfo.AttachmentReferences[0].attachment = 0;
        renderPassInfo.AttachmentReferences[0].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        renderPassInfo.Subpasses.resize(1);
        
        renderPassInfo.Subpasses[0].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        renderPassInfo.Subpasses[0].colorAttachmentCount = 1;
        renderPassInfo.Subpasses[0].pColorAttachments = &renderPassInfo.AttachmentReferences[0];

        return renderPassInfo;
    }
}
