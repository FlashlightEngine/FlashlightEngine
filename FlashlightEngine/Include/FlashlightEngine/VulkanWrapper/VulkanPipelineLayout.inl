#pragma once

#include "FlashlightEngine/Core/Log.hpp"

inline VulkanPipelineLayout::VulkanPipelineLayout() : m_Device(VK_NULL_HANDLE), m_PipelineLayout(VK_NULL_HANDLE) {}

inline VulkanPipelineLayout::~VulkanPipelineLayout() {
    if (IsValid()) {
        Destroy();
    }
}

inline bool VulkanPipelineLayout::IsValid() const noexcept {
    return m_PipelineLayout != nullptr;
}

inline VkPipelineLayout VulkanPipelineLayout::GetPipelineLayout() const noexcept {
    return m_PipelineLayout;
}

inline void VulkanPipelineLayout::Create(const VulkanDevice& device, const VulkanPipelineLayoutInfo &layoutInfo) {
    VkPipelineLayoutCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    createInfo.setLayoutCount = static_cast<u32>(layoutInfo.SetLayouts.size());
    createInfo.pSetLayouts = layoutInfo.SetLayouts.data();
    createInfo.pushConstantRangeCount = static_cast<u32>(layoutInfo.PushConstantRanges.size());
    createInfo.pPushConstantRanges = layoutInfo.PushConstantRanges.data();

    if (vkCreatePipelineLayout(device.GetDevice(), &createInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS) {
        FL_ERROR("Failed to create pipeline layout.")
    }

    m_Device = device.GetDevice();
}

inline void VulkanPipelineLayout::Destroy() {
    if (IsValid()) {
        vkDestroyPipelineLayout(m_Device, m_PipelineLayout, nullptr);
    }
}
