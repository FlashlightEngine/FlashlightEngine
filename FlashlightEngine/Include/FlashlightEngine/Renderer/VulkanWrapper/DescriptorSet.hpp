#pragma once

#include "FlashlightEngine/Renderer/VulkanWrapper/Device.hpp"

namespace Flashlight::VulkanWrapper {
    struct DescriptorSetLayoutEntry {
        u32 BindingNumber;
        u32 BindingCount;
        VkDescriptorType DescriptorType;
        VkPipelineStageFlags Stages;
    };

    class DescriptorSetLayout {
        VkDescriptorSetLayout m_Layout = VK_NULL_HANDLE;

        const Device& m_Device;

    public:
        class Builder {
            std::vector<DescriptorSetLayoutEntry> m_Entries;
            
        public:
            Builder& AddEntry(const DescriptorSetLayoutEntry& entry);
            [[nodiscard]] std::unique_ptr<DescriptorSetLayout> Build(const Device& device) const;
        };
        
        DescriptorSetLayout(const Device& device, const std::vector<VkDescriptorSetLayoutBinding>& bindings);
        ~DescriptorSetLayout();

        DescriptorSetLayout(const DescriptorSetLayout&) = delete;
        DescriptorSetLayout(DescriptorSetLayout&& rhs) noexcept;

        DescriptorSetLayout& operator=(const DescriptorSetLayout&) = delete;
        DescriptorSetLayout& operator=(DescriptorSetLayout&& rhs) noexcept;

        [[nodiscard]] inline VkDescriptorSetLayout GetNativeLayout() const;
    };

#include "DescriptorSet.inl"
}