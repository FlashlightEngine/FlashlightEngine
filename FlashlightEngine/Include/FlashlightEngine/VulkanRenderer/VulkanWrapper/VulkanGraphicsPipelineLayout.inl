#pragma once

inline VulkanGraphicsPipelineLayout::VulkanGraphicsPipelineLayout() : m_Device(VK_NULL_HANDLE), m_PipelineLayout(VK_NULL_HANDLE) {}

inline VulkanGraphicsPipelineLayout::~VulkanGraphicsPipelineLayout() {
    if (IsValid()) {
        Destroy();
    }
}

inline bool VulkanGraphicsPipelineLayout::IsValid() const noexcept {
    return m_PipelineLayout != nullptr;
}

inline VkPipelineLayout VulkanGraphicsPipelineLayout::GetPipelineLayout() const noexcept {
    return m_PipelineLayout;
}

inline void VulkanGraphicsPipelineLayout::Create(const VulkanDevice& device, const VulkanPipelineLayoutInfo &layoutInfo) {
    VkPipelineLayoutCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    createInfo.setLayoutCount = static_cast<u32>(layoutInfo.SetLayouts.size());
    createInfo.pSetLayouts = layoutInfo.SetLayouts.data();
    createInfo.pushConstantRangeCount = static_cast<u32>(layoutInfo.PushConstantRanges.size());
    createInfo.pPushConstantRanges = layoutInfo.PushConstantRanges.data();

    if (vkCreatePipelineLayout(device.GetHandle(), &createInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS) {
        std::cerr << "Failed to create pipeline layout.";
        throw std::runtime_error("");
    }

    m_Device = device.GetHandle();
}

inline void VulkanGraphicsPipelineLayout::Destroy() {
    if (IsValid()) {
        vkDestroyPipelineLayout(m_Device, m_PipelineLayout, nullptr);
    }
}
