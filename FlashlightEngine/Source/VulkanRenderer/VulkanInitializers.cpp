/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : VulkanInitializers.cpp
 * Description : Definitions of methods from VulkanInitializers.hpp.
 */
#include <FlashlightEngine/VulkanRenderer/VulkanInitializers.hpp>

namespace Flashlight::VulkanRenderer::VulkanInit {
    VkCommandPoolCreateInfo
    CommandPoolCreateInfo(const u32 queueFamilyIndex, const VkCommandPoolCreateFlags flags) {
        VkCommandPoolCreateInfo info;
        info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        info.pNext = nullptr;
        info.queueFamilyIndex = queueFamilyIndex;
        info.flags = flags;
        return info;
    }

    VkCommandBufferAllocateInfo CommandBufferAllocateInfo(const VkCommandPool pool, const u32 count) {
        VkCommandBufferAllocateInfo info;
        info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        info.pNext = nullptr;

        info.commandPool = pool;
        info.commandBufferCount = count;
        info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        return info;
    }

    VkCommandBufferBeginInfo CommandBufferBeginInfo(const VkCommandBufferUsageFlags flags) {
        VkCommandBufferBeginInfo info;
        info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        info.pNext = nullptr;

        info.pInheritanceInfo = nullptr;
        info.flags = flags;
        return info;
    }

    VkCommandBufferSubmitInfo CommandBufferSubmitInfo(const VkCommandBuffer commandBuffer) {
        VkCommandBufferSubmitInfo info;
        info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
        info.pNext = nullptr;

        info.commandBuffer = commandBuffer;
        info.deviceMask = 0;

        return info;
    }

    VkFenceCreateInfo FenceCreateInfo(const VkFenceCreateFlags flags) {
        VkFenceCreateInfo info;
        info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        info.pNext = nullptr;

        info.flags = flags;
        return info;
    }

    VkSemaphoreCreateInfo SemaphoreCreateInfo(const VkSemaphoreCreateFlags flags) {
        VkSemaphoreCreateInfo info;
        info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        info.pNext = nullptr;

        info.flags = flags;

        return info;
    }


    VkSubmitInfo2 SubmitInfo(const VkCommandBufferSubmitInfo* commandBufferSubmitInfo,
                             const VkSemaphoreSubmitInfo* signalSemaphoreInfo,
                             const VkSemaphoreSubmitInfo* waitSemaphoreInfo) {
        VkSubmitInfo2 info{};
        info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
        info.pNext = nullptr;

        info.waitSemaphoreInfoCount = waitSemaphoreInfo == nullptr ? 0 : 1;
        info.pWaitSemaphoreInfos = waitSemaphoreInfo;

        info.signalSemaphoreInfoCount = signalSemaphoreInfo == nullptr ? 0 : 1;
        info.pSignalSemaphoreInfos = signalSemaphoreInfo;

        info.commandBufferInfoCount = 1;
        info.pCommandBufferInfos = commandBufferSubmitInfo;

        return info;
    }

    VkPresentInfoKHR PresentInfo() {
        VkPresentInfoKHR info{};
        info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        info.pNext = nullptr;

        info.swapchainCount = 0;
        info.pSwapchains = nullptr;
        info.pWaitSemaphores = nullptr;
        info.waitSemaphoreCount = 0;
        info.pImageIndices = nullptr;

        return info;
    }

    VkRenderingAttachmentInfo AttachmentInfo(const VkImageView view, const VkClearValue* clearValue,
                                             const VkImageLayout layout) {
        VkRenderingAttachmentInfo colorAttachment{};
        colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
        colorAttachment.pNext = nullptr;

        colorAttachment.imageView = view;
        colorAttachment.imageLayout = layout;
        colorAttachment.loadOp = clearValue ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        if (clearValue) {
            colorAttachment.clearValue = *clearValue;
        }

        return colorAttachment;
    }

    VkRenderingAttachmentInfo DepthAttachmentInfo(const VkImageView view, const VkImageLayout layout) {
        VkRenderingAttachmentInfo depthAttachment{};
        depthAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
        depthAttachment.pNext = nullptr;

        depthAttachment.imageView = view;
        depthAttachment.imageLayout = layout;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        depthAttachment.clearValue.depthStencil.depth = 0.0f;

        return depthAttachment;
    }

    VkRenderingInfo RenderingInfo(const VkExtent2D renderExtent, const VkRenderingAttachmentInfo* colorAttachment,
                                  const VkRenderingAttachmentInfo* depthAttachment) {
        VkRenderingInfo renderInfo{};
        renderInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
        renderInfo.pNext = nullptr;

        renderInfo.renderArea = VkRect2D{VkOffset2D{0, 0}, renderExtent};
        renderInfo.layerCount = 1;
        renderInfo.colorAttachmentCount = 1;
        renderInfo.pColorAttachments = colorAttachment;
        renderInfo.pDepthAttachment = depthAttachment;
        renderInfo.pStencilAttachment = nullptr;

        return renderInfo;
    }

    VkImageSubresourceRange ImageSubresourceRange(const VkImageAspectFlags aspectMask) {
        VkImageSubresourceRange subImage;
        subImage.aspectMask = aspectMask;
        subImage.baseMipLevel = 0;
        subImage.levelCount = VK_REMAINING_MIP_LEVELS;
        subImage.baseArrayLayer = 0;
        subImage.layerCount = VK_REMAINING_ARRAY_LAYERS;

        return subImage;
    }

    VkSemaphoreSubmitInfo SemaphoreSubmitInfo(const VkPipelineStageFlags2 stageMask, const VkSemaphore semaphore) {
        VkSemaphoreSubmitInfo submitInfo;
        submitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
        submitInfo.pNext = nullptr;

        submitInfo.semaphore = semaphore;
        submitInfo.stageMask = stageMask;
        submitInfo.deviceIndex = 0;
        submitInfo.value = 1;

        return submitInfo;
    }

    VkDescriptorSetLayoutBinding DescriptorSetLayoutBinding(const VkDescriptorType type,
                                                            const VkShaderStageFlags stageFlags,
                                                            const u32 binding) {
        VkDescriptorSetLayoutBinding setBind;
        setBind.binding = binding;
        setBind.descriptorCount = 1;
        setBind.descriptorType = type;
        setBind.pImmutableSamplers = nullptr;
        setBind.stageFlags = stageFlags;

        return setBind;
    }

    VkDescriptorSetLayoutCreateInfo DescriptorSetLayoutCreateInfo(const VkDescriptorSetLayoutBinding* bindings,
                                                                  const u32 bindingCount) {
        VkDescriptorSetLayoutCreateInfo info;
        info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        info.pNext = nullptr;

        info.pBindings = bindings;
        info.bindingCount = bindingCount;
        info.flags = 0;

        return info;
    }

    VkWriteDescriptorSet WriteDescriptorImage(const VkDescriptorType type, const VkDescriptorSet dstSet,
                                              const VkDescriptorImageInfo* imageInfo, const u32 binding) {
        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.pNext = nullptr;

        write.dstBinding = binding;
        write.dstSet = dstSet;
        write.descriptorCount = 1;
        write.descriptorType = type;
        write.pImageInfo = imageInfo;

        return write;
    }

    VkWriteDescriptorSet WriteDescriptorBuffer(const VkDescriptorType type, const VkDescriptorSet dstSet,
                                               const VkDescriptorBufferInfo* bufferInfo, const u32 binding) {
        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.pNext = nullptr;

        write.dstBinding = binding;
        write.dstSet = dstSet;
        write.descriptorCount = 1;
        write.descriptorType = type;
        write.pBufferInfo = bufferInfo;

        return write;
    }

    VkDescriptorBufferInfo BufferInfo(const VkBuffer buffer, const VkDeviceSize offset, const VkDeviceSize range) {
        VkDescriptorBufferInfo bufferInfo;
        bufferInfo.buffer = buffer;
        bufferInfo.offset = offset;
        bufferInfo.range = range;

        return bufferInfo;
    }

    VkImageCreateInfo ImageCreateInfo(const VkFormat format, const VkImageUsageFlags usageFlags,
                                      const VkExtent3D extent) {
        VkImageCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        info.pNext = nullptr;

        info.imageType = VK_IMAGE_TYPE_2D;

        info.format = format;
        info.extent = extent;

        info.mipLevels = 1;
        info.arrayLayers = 1;

        info.samples = VK_SAMPLE_COUNT_1_BIT;

        // Optimal tiling, which means the image is stored on the best gpu format.
        info.tiling = VK_IMAGE_TILING_OPTIMAL;
        info.usage = usageFlags;

        return info;
    }

    VkImageViewCreateInfo ImageViewCreateInfo(const VkFormat format, const VkImage image,
                                              const VkImageAspectFlags aspectFlags) {
        VkImageViewCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        info.pNext = nullptr;

        info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        info.image = image;
        info.format = format;
        info.subresourceRange.baseMipLevel = 0;
        info.subresourceRange.levelCount = 1;
        info.subresourceRange.baseArrayLayer = 0;
        info.subresourceRange.layerCount = 1;
        info.subresourceRange.aspectMask = aspectFlags;

        return info;
    }

    VkPipelineLayoutCreateInfo PipelineLayoutCreateInfo() {
        VkPipelineLayoutCreateInfo info;
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        info.pNext = nullptr;

        info.flags = 0;
        info.setLayoutCount = 0;
        info.pSetLayouts = nullptr;
        info.pushConstantRangeCount = 0;
        info.pPushConstantRanges = nullptr;

        return info;
    }

    VkPipelineShaderStageCreateInfo PipelineShaderStageCreateInfo(const VkShaderStageFlagBits stage,
                                                                  const VkShaderModule shaderModule,
                                                                  const char* entry) {
        VkPipelineShaderStageCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        info.pNext = nullptr;

        info.stage = stage;
        info.module = shaderModule;
        info.pName = entry;

        return info;
    }
}
