#pragma once

inline VkDescriptorSetLayout DescriptorSetLayout::GetNativeLayout() const {
    return m_Layout;
}

inline VkDescriptorPool DescriptorPool::GetNativePool() const {
    return m_Pool;
}
