#pragma once

#include "FlashlightEngine/Renderer/VulkanWrapper/Device.hpp"

namespace Flashlight::VulkanWrapper {
    class DescriptorSetLayout {
        VkDescriptorSetLayoutBinding m_UboLayoutBinding;
        VkDescriptorSetLayout m_Layout;

        const Device& m_Device;        
    public:
        DescriptorSetLayout(const Device& device, VkPipelineStageFlags stages);
        ~DescriptorSetLayout();

        DescriptorSetLayout(const DescriptorSetLayout&) = delete;
        DescriptorSetLayout(DescriptorSetLayout&&) = delete;

        DescriptorSetLayout& operator=(const DescriptorSetLayout&) = delete;
        DescriptorSetLayout& operator=(DescriptorSetLayout&&) = delete;

        [[nodiscard]] inline  VkDescriptorSetLayoutBinding GetUboBinding() const;
        [[nodiscard]] inline  VkDescriptorSetLayout GetLayout() const;
    };

#include "DescriptorSet.inl"
}
