#pragma once

inline VkDescriptorSetLayoutBinding DescriptorSetLayout::GetUboBinding() const {
    return m_UboLayoutBinding;
}

inline VkDescriptorSetLayout DescriptorSetLayout::GetLayout() const {
    return m_Layout;
}
