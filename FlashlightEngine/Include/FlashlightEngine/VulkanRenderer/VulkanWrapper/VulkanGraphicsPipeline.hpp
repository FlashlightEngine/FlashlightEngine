#pragma once

#include "VulkanDevice.hpp"
#include "VulkanPipelineLayout.hpp"
#include "VulkanShaderModule.hpp"

#include <volk.h>

namespace Flashlight {

namespace VulkanWrapper {

    struct VulkanGraphicsPipelineStages {
        std::vector<VkDynamicState> DynamicStates;
        
        VkPipelineDynamicStateCreateInfo DynamicStatesInfo;
        VkPipelineVertexInputStateCreateInfo VertexInput;
        VkPipelineInputAssemblyStateCreateInfo InputAssembly;
        VkPipelineRasterizationStateCreateInfo Rasterization;
        VkPipelineMultisampleStateCreateInfo Multisampling;
        VkPipelineDepthStencilStateCreateInfo DepthStencil;
        VkPipelineColorBlendAttachmentState ColorAttachment;
        VkPipelineColorBlendStateCreateInfo ColorBlend;
        
        inline void UseDefaultPipelineConfiguration(const std::vector<VkVertexInputAttributeDescription> &vertexAttributeDescriptions,
                                                    const std::vector<VkVertexInputBindingDescription> &vertexBindingDescriptions);
    };
    
    class VulkanGraphicsPipeline {
    public:
        inline VulkanGraphicsPipeline();
        VulkanGraphicsPipeline(const VulkanGraphicsPipeline &) = delete;
        VulkanGraphicsPipeline(VulkanGraphicsPipeline &&) = delete;
        inline ~VulkanGraphicsPipeline();

        inline bool IsValid() const noexcept;
        inline bool GetHandle() const noexcept;

        inline void Create(const VulkanDevice &device, const VulkanGraphicsPipelineStages &pipelineStages,
                           const VulkanPipelineLayout &layout, const VulkanShaderModule &vertexShader,
                           const VulkanShaderModule &fragmentShader);
        inline void Destroy() const;

    private:
        VkDevice m_Device;
        VkPipeline m_Pipeline;
    };

#include "VulkanGraphicsPipeline.inl"
    
}

}
