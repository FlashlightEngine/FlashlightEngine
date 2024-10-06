// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.h

#include "FlashlightEngine/Renderer/Vulkan/VulkanRenderPass.h"

#include "FlashlightEngine/Core/FlMemory.h"

void flVulkanRenderPassCreate(
    FlVulkanContext* context,
    FlVulkanRenderPass* outRenderPass,
    FlFloat32 x, FlFloat32 y, FlFloat32 w, FlFloat32 h,
    FlFloat32 r, FlFloat32 g, FlFloat32 b, FlFloat32 a,
    FlFloat32 depth,
    FlUInt32 stencil
) {
    outRenderPass->X = x;
    outRenderPass->Y = y;
    outRenderPass->W = w;
    outRenderPass->H = h;

    outRenderPass->R = r;
    outRenderPass->G = g;
    outRenderPass->B = b;
    outRenderPass->A = a;

    outRenderPass->Depth = depth;
    outRenderPass->Stencil = stencil;

    // Main subpass.
    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    
    // Attachments. TODO: Make this configurable.
    const FlUInt32 attachmentDescriptionCount = 2;
    VkAttachmentDescription attachmentDescriptions[attachmentDescriptionCount];

    // Color attachment.
     // TODO: Make this configurable.
    VkAttachmentDescription colorAttachment;
    colorAttachment.format = context->Swapchain.ImageFormat.format;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; // Do not expect any particular layout before the render pass starts.
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; // Transitioned to after the renderpass.
    colorAttachment.flags = 0;

    attachmentDescriptions[0] = colorAttachment;
    
    VkAttachmentReference colorAttachmentReference;
    colorAttachmentReference.attachment = 0; // Attachment description array index.
    colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentReference;

    // Depth attachment, if there is one.
    VkAttachmentDescription depthAttachment;
    depthAttachment.format = context->Device.DepthFormat;
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    depthAttachment.flags = 0;

    attachmentDescriptions[1] = depthAttachment;

    // Depth attachment reference.
    VkAttachmentReference depthAttachmentReference;
    depthAttachmentReference.attachment = 1;
    depthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    subpass.pDepthStencilAttachment = &depthAttachmentReference;

    // Input from a shader.
    subpass.inputAttachmentCount = 0;
    subpass.pInputAttachments = 0;

    // Attachments used for multisampling color attachments.
    subpass.pResolveAttachments = 0;

    // Attachments that are not used for this subpass, but must be preserved for the next.
    subpass.preserveAttachmentCount = 0;
    subpass.pPreserveAttachments = 0;

    // TODO: Make configurable.
    // Render pass dependencies.
    VkSubpassDependency dependency;
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependency.dependencyFlags = 0;

    VkRenderPassCreateInfo renderPassInfo = {VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO};
    renderPassInfo.attachmentCount = attachmentDescriptionCount;
    renderPassInfo.pAttachments = attachmentDescriptions;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;
    renderPassInfo.pNext = 0;
    renderPassInfo.flags = 0;

    VK_CHECK(vkCreateRenderPass(
        context->Device.LogicalDevice,
        &renderPassInfo,
        context->Allocator,
        &outRenderPass->Handle
    ))    
}

void flVulkanRenderPassDestroy(FlVulkanContext* context, FlVulkanRenderPass* renderPass) {
    if (renderPass && renderPass->Handle) {
        vkDestroyRenderPass(context->Device.LogicalDevice, renderPass->Handle, context->Allocator);
        renderPass->Handle = VK_NULL_HANDLE;
    }
}

void flVulkanRenderPassBegin(
    FlVulkanCommandBuffer* commandBuffer,
    FlVulkanRenderPass* renderPass,
    VkFramebuffer framebuffer
) {
    VkRenderPassBeginInfo beginInfo = {VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO};
    beginInfo.renderPass = renderPass->Handle;
    beginInfo.framebuffer = framebuffer;
    beginInfo.renderArea.offset.x = renderPass->X;
    beginInfo.renderArea.offset.y = renderPass->Y;
    beginInfo.renderArea.extent.width = renderPass->W;
    beginInfo.renderArea.extent.height = renderPass->H;
    
    VkClearValue clearValues[2];
    clearValues[0].color.float32[0] = renderPass->R;
    clearValues[0].color.float32[1] = renderPass->G;
    clearValues[0].color.float32[2] = renderPass->B;
    clearValues[0].color.float32[3] = renderPass->A;

    clearValues[1].depthStencil.depth = renderPass->Depth;
    clearValues[1].depthStencil.stencil = renderPass->Stencil;

    beginInfo.clearValueCount = 2;
    beginInfo.pClearValues = clearValues;

    vkCmdBeginRenderPass(commandBuffer->Handle, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);
    commandBuffer->State = FlVulkanCommandBufferStateInRenderPass;
}

void flVulkanRenderPassEnd(FlVulkanCommandBuffer* commandBuffer, FlVulkanRenderPass* renderPass) {
    vkCmdEndRenderPass(commandBuffer->Handle);
    commandBuffer->State = FlVulkanCommandBufferStateRecording;
}
