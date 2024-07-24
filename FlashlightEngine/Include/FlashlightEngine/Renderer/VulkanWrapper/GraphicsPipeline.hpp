/* Copyright (C) 2024 Jean "Pixfri" Letessier
* This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : GraphicsPipeline.hpp
 * Description : Declaration of the GraphicsPipeline class. The graphics pipeline is the set of steps required for
 *               rendering.
 */
#pragma once

#include "FlashlightEngine/Renderer/VulkanWrapper/Device.hpp"
#include "FlashlightEngine/Renderer/VulkanWrapper/RenderPass.hpp"
#include "FlashlightEngine/Renderer/VulkanWrapper/ShaderModule.hpp"

namespace Flashlight::VulkanWrapper {
    struct PipelineInfos {
        std::filesystem::path VertexShaderPath;
        std::filesystem::path FragmentShaderPath;

        VkPipelineVertexInputStateCreateInfo VertexInputInfo;
        VkPipelineInputAssemblyStateCreateInfo InputAssemblyInfo;
        std::vector<VkDynamicState> DynamicStates;
        VkPipelineDynamicStateCreateInfo DynamicStatesInfo;
        VkPipelineViewportStateCreateInfo ViewportStateInfo;
        VkPipelineRasterizationStateCreateInfo RasterizationInfo;
        VkPipelineMultisampleStateCreateInfo MultisamplingInfo;
        VkPipelineColorBlendAttachmentState ColorBlendAttachment;
        VkPipelineColorBlendStateCreateInfo ColorBlendingInfo;
        VkPipelineLayoutCreateInfo PipelineLayoutInfo;

        VkRenderPass RenderPass;
        u32 SubpassIndex = 0; // The subpass index for the pipeline is 0 when using the default settings.
    };

    class GraphicsPipeline {
        VkPipeline m_Pipeline = VK_NULL_HANDLE;
        VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;

        Device& m_Device;

    public:
        class Builder {
        public:
            explicit inline Builder(Device& device);

            void VertexShader(ShaderModule& shaderModule);
            void FragmentShader(ShaderModule& shaderModule);
            void VertexInputState(const std::vector<VkVertexInputBindingDescription>& bindingDescriptions,
                                  const std::vector<VkVertexInputAttributeDescription>& attributeDescriptions);
            void InputAssemblyState(VkPrimitiveTopology topology, bool primitiveRestartEnabled);
            void Viewport(VkExtent2D extent);
            void RasterizeState(VkPolygonMode polygonMode, VkCullModeFlagBits cullMode);
            void MultisampleState();
            void ColorBlendState(bool blendEnabled, VkBlendOp colorBlendingOp, VkBlendOp alphaBlendingOp,
                                 bool logicOpEnabled, VkLogicOp logicOp);
            void PipelineLayout(const std::vector<VkDescriptorSetLayout>& layouts,
                                     const std::vector<VkPushConstantRange>& pushConstantRanges);

            std::unique_ptr<GraphicsPipeline> Build(const RenderPass& renderPass) const;

        private:
            Device& m_Device;

            ShaderModule* m_VertexShaderModule;
            ShaderModule* m_FragmentShaderModule;

            VkPipelineVertexInputStateCreateInfo m_VertexInputInfo{};
            VkPipelineInputAssemblyStateCreateInfo m_InputAssemblyInfo{};
            VkViewport m_Viewport{};
            VkRect2D m_Scissor{};
            VkPipelineViewportStateCreateInfo m_ViewportStateInfo{};
            VkPipelineRasterizationStateCreateInfo m_RasterizationInfo{};
            VkPipelineMultisampleStateCreateInfo m_MultisamplingInfo{};
            VkPipelineColorBlendAttachmentState m_ColorBlendAttachment{};
            VkPipelineColorBlendStateCreateInfo m_ColorBlendingInfo{};
            VkPipelineLayoutCreateInfo m_PipelineLayoutCreateInfo{};
        };

        GraphicsPipeline(Device& device, const VkPipeline& pipeline, const VkPipelineLayout& pipelineLayout);
        inline ~GraphicsPipeline();

        GraphicsPipeline(const GraphicsPipeline&) = delete;
        GraphicsPipeline(GraphicsPipeline&&) = delete;

        GraphicsPipeline& operator=(const GraphicsPipeline&) = delete;
        GraphicsPipeline& operator=(GraphicsPipeline&&) = delete;

        [[nodiscard]] inline VkPipeline GetNativePipeline() const;
        [[nodiscard]] inline VkPipelineLayout GetNativePipelineLayout() const;
    };

#include "GraphicsPipeline.inl"
}
