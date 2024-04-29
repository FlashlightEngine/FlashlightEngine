#pragma once

#include "VulkanDevice.hpp"

#include <volk.h>

namespace Flashlight {

namespace VulkanWrapper {

    struct VulkanPipelineLayoutInfo {
        std::vector<VkDescriptorSetLayout> SetLayouts;
        std::vector<VkPushConstantRange> PushConstantRanges;
    };
    
    class VulkanGraphicsPipelineLayout {
    public:
        inline VulkanGraphicsPipelineLayout();
        VulkanGraphicsPipelineLayout(const VulkanGraphicsPipelineLayout &) = delete;
        VulkanGraphicsPipelineLayout(VulkanGraphicsPipelineLayout &&) = delete;
        inline ~VulkanGraphicsPipelineLayout();
 
        inline bool IsValid() const noexcept;
        inline VkPipelineLayout GetPipelineLayout() const noexcept;
         
        inline void Create(const VulkanDevice &device, const VulkanPipelineLayoutInfo &layoutInfo);
        inline void Destroy();

    private:
        VkDevice m_Device;
        VkPipelineLayout m_PipelineLayout;
    };

#include "VulkanGraphicsPipelineLayout.inl"
    
}

}
