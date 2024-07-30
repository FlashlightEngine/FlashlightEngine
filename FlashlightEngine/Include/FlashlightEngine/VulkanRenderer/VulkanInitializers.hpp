/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : VulkanInitializers.hpp
 * Description : Declaration of methods used to initialize Vulkan structures.
 */
#pragma once

#include <FlashlightEngine/VulkanRenderer/VulkanTypes.hpp>

namespace Flashlight::VulkanRenderer::VulkanInit {
    VkCommandPoolCreateInfo CommandPoolCreateInfo(u32 queueFamilyIndex, VkCommandPoolCreateFlags flags = 0);
    VkCommandBufferAllocateInfo CommandBufferAllocateInfo(VkCommandPool pool, u32 count = 1);

    VkCommandBufferBeginInfo CommandBufferBeginInfo(VkCommandBufferUsageFlags flags = 0);
    VkCommandBufferSubmitInfo CommandBufferSubmitInfo(VkCommandBuffer commandBuffer);

    VkFenceCreateInfo FenceCreateInfo(VkFenceCreateFlags flags = 0);

    VkSemaphoreCreateInfo SemaphoreCreateInfo(VkSemaphoreCreateFlags flags = 0);


    VkSubmitInfo2 SubmitInfo(const VkCommandBufferSubmitInfo* commandBufferSubmitInfo,
                             const VkSemaphoreSubmitInfo* signalSemaphoreInfo,
                             const VkSemaphoreSubmitInfo* waitSemaphoreInfo);
    VkPresentInfoKHR PresentInfo();

    VkRenderingAttachmentInfo AttachmentInfo(VkImageView view, const VkClearValue* clearValue, VkImageLayout layout);

    VkRenderingAttachmentInfo DepthAttachmentInfo(VkImageView view, VkImageLayout layout);

    VkRenderingInfo RenderingInfo(VkExtent2D renderExtent, const VkRenderingAttachmentInfo* colorAttachment,
                                  const VkRenderingAttachmentInfo* depthAttachment);

    VkImageSubresourceRange ImageSubresourceRange(VkImageAspectFlags aspectMask);

    VkSemaphoreSubmitInfo SemaphoreSubmitInfo(VkPipelineStageFlags2 stageMask, VkSemaphore semaphore);

    VkDescriptorSetLayoutBinding DescriptorSetLayoutBinding(VkDescriptorType type, VkShaderStageFlags stageFlags,
                                                            u32 binding);
    VkDescriptorSetLayoutCreateInfo DescriptorSetLayoutCreateInfo(const VkDescriptorSetLayoutBinding* bindings,
                                                                  u32 bindingCount);
    VkWriteDescriptorSet WriteDescriptorImage(VkDescriptorType type, VkDescriptorSet dstSet,
                                              const VkDescriptorImageInfo* imageInfo, u32 binding);
    VkWriteDescriptorSet WriteDescriptorBuffer(VkDescriptorType type, VkDescriptorSet dstSet,
                                               const VkDescriptorBufferInfo* bufferInfo, u32 binding);
    VkDescriptorBufferInfo BufferInfo(VkBuffer buffer, VkDeviceSize offset, VkDeviceSize range);

    VkImageCreateInfo ImageCreateInfo(VkFormat format, VkImageUsageFlags usageFlags, VkExtent3D extent);
    VkImageViewCreateInfo ImageViewCreateInfo(VkFormat format, VkImage image, VkImageAspectFlags aspectFlags);
    VkPipelineLayoutCreateInfo PipelineLayoutCreateInfo();
    VkPipelineShaderStageCreateInfo PipelineShaderStageCreateInfo(VkShaderStageFlagBits stage,
                                                                  VkShaderModule shaderModule,
                                                                  const char* entry = "main");
}
