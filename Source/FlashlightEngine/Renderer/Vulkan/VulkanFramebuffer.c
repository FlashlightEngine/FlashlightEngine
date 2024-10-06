// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.h

#include "FlashlightEngine/Renderer/Vulkan/VulkanFramebuffer.h"

#include "FlashlightEngine/Core/FlMemory.h"

void flVulkanFramebufferCreate(
    FlVulkanContext* context,
    FlVulkanRenderPass* renderPass,
    FlUInt32 width,
    FlUInt32 height,
    FlUInt32 attachmentCount,
    VkImageView* attachments,
    FlVulkanFramebuffer* outFramebuffer
) {
    // Take a copy of the attachments, renderpass and attachment count.
    outFramebuffer->Attachments = (VkImageView*)flAllocate(sizeof(VkImageView) * attachmentCount, FlMemoryTagRenderer);
    for (FlUInt32 i = 0; i < attachmentCount; ++i) {
        outFramebuffer->Attachments[i] = attachments[i];
    }
    outFramebuffer->RenderPass = renderPass;
    outFramebuffer->AttachmentCount = attachmentCount;

    VkFramebufferCreateInfo framebufferInfo = {VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO};
    framebufferInfo.renderPass = outFramebuffer->RenderPass->Handle;
    framebufferInfo.attachmentCount = outFramebuffer->AttachmentCount;
    framebufferInfo.pAttachments = outFramebuffer->Attachments;
    framebufferInfo.width = width;
    framebufferInfo.height = height;
    framebufferInfo.layers = 1;

    VK_CHECK(vkCreateFramebuffer(context->Device.LogicalDevice, &framebufferInfo, context->Allocator, &outFramebuffer->Handle))
}

void flVulkanFramebufferDestroy(FlVulkanContext* context, FlVulkanFramebuffer* framebuffer) {
    vkDestroyFramebuffer(context->Device.LogicalDevice, framebuffer->Handle, context->Allocator);
    if (framebuffer->Attachments) {
        flFree(framebuffer->Attachments, sizeof(VkImageView) * framebuffer->AttachmentCount, FlMemoryTagRenderer);
        framebuffer->Attachments = 0;
    }
    framebuffer->Handle = VK_NULL_HANDLE;
    framebuffer->AttachmentCount = 0;
    framebuffer->RenderPass = 0;
}
