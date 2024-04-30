#pragma once

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
        std::cerr << "Failed to create render pass.";
        throw std::runtime_error("");
    }

    m_Device = device.GetHandle();
}

inline void VulkanRenderPass::Destroy() const {
    if (IsValid()) {
        vkDestroyRenderPass(m_Device, m_Handle, nullptr);
    }
}

