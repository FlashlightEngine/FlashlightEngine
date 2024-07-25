#pragma once

inline VkDescriptorSetLayout DescriptorSetLayout::GetNativeLayout() const {
    return m_Layout;
}
