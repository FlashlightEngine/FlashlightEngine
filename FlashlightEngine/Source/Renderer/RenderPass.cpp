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

    void RenderPass::UseDefaultRenderPassInfo(RenderPassInfo& renderPassInfo, const VulkanWrapper::SwapChain& swapChain) {
        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = swapChain.GetSwapChainImageFormat();
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        renderPassInfo.Attachments = {colorAttachment};

        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0; // The first (and only) attachment (the color attachment) index, used for the location.
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        renderPassInfo.AttachmentReferences = {colorAttachmentRef};

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;

        renderPassInfo.Subpasses = {subpass};
    }
}
