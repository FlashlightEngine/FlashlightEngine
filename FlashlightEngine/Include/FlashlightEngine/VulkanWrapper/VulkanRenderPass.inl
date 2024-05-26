#pragma once

inline void VulkanRenderPassAttachments::UseDefaultAttachments(VulkanRenderPassAttachments &attachments, const VkFormat &format) {
    attachments.ColorAttachmentDescription.format = format;
    attachments.ColorAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;

    attachments.ColorAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments.ColorAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

    attachments.ColorAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachments.ColorAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    attachments.ColorAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachments.ColorAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    attachments.ColorAttachmentReference.attachment = 0;
    attachments.ColorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
}


inline VulkanRenderPass::VulkanRenderPass() : m_Device(VK_NULL_HANDLE), m_Handle(VK_NULL_HANDLE) {}

inline VulkanRenderPass::~VulkanRenderPass() {
    if (IsValid()) {
        Destroy();
    }
}

inline bool VulkanRenderPass::IsValid() const noexcept {
    return m_Handle != nullptr;
}

inline VkRenderPass VulkanRenderPass::GetHandle() const noexcept {
    return m_Handle;
}

inline void VulkanRenderPass::Create(const VulkanDevice& device, const VulkanRenderPassAttachments& attachments) {
    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_COMPUTE;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &attachments.ColorAttachmentReference;

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &attachments.ColorAttachmentDescription;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;

    if (vkCreateRenderPass(device.GetHandle(), &renderPassInfo, nullptr, &m_Handle) != VK_SUCCESS) {
        FL_ERROR("Failed to create render pass")
    }

    m_Device = device.GetHandle();
}

inline void VulkanRenderPass::Destroy() const {
    if (IsValid()) {
        vkDestroyRenderPass(m_Device, m_Handle, nullptr);
    }
}

