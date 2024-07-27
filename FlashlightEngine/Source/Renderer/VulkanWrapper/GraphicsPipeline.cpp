/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : GraphicsPipeline.cpp
 * Description : Definitions of methods from the GraphicsPipeline class.
 */
#include "FlashlightEngine/Renderer/VulkanWrapper/GraphicsPipeline.hpp"

#include "FlashlightEngine/Renderer/VulkanWrapper/ShaderModule.hpp"
#include "FlashlightEngine/Renderer/VulkanWrapper/RenderPass.hpp"

namespace Flashlight::VulkanWrapper {
    GraphicsPipeline::GraphicsPipeline(const std::shared_ptr<Device>& device, const VkPipeline& pipeline,
                                       const VkPipelineLayout& pipelineLayout) : m_Pipeline(pipeline),
                                                                                 m_PipelineLayout(pipelineLayout),
                                                                                 m_Device(device) {
    }

    void GraphicsPipeline::Builder::VertexShader(const std::shared_ptr<ShaderModule>& shaderModule) {
        m_VertexShaderModule = shaderModule;
    }

    void GraphicsPipeline::Builder::FragmentShader(const std::shared_ptr<ShaderModule>& shaderModule) {
        m_FragmentShaderModule = shaderModule;
    }

    void GraphicsPipeline::Builder::VertexInputState(
        const std::vector<VkVertexInputBindingDescription>& bindingDescriptions,
        const std::vector<VkVertexInputAttributeDescription>& attributeDescriptions) {
        m_VertexInputInfo = {};
        m_VertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        m_VertexInputInfo.vertexBindingDescriptionCount = static_cast<u32>(bindingDescriptions.size());
        m_VertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();
        m_VertexInputInfo.vertexAttributeDescriptionCount = static_cast<u32>(attributeDescriptions.size());
        m_VertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
    }

    void GraphicsPipeline::Builder::InputAssemblyState(const VkPrimitiveTopology topology,
                                                       const bool primitiveRestartEnabled) {
        m_InputAssemblyInfo = {};
        m_InputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        m_InputAssemblyInfo.topology = topology;
        m_InputAssemblyInfo.primitiveRestartEnable = primitiveRestartEnabled;
    }

    void GraphicsPipeline::Builder::Viewport(const VkExtent2D extent) {
        m_Viewport = {};
        m_Viewport.x = 0;
        m_Viewport.y = 0;
        m_Viewport.width = static_cast<float>(extent.width);
        m_Viewport.height = static_cast<float>(extent.height);
        m_Viewport.minDepth = 0;
        m_Viewport.maxDepth = 0;

        m_Scissor = {};
        m_Scissor.offset = {0, 0};
        m_Scissor.extent = extent;

        m_ViewportStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        m_ViewportStateInfo.viewportCount = 1;
        m_ViewportStateInfo.pViewports = &m_Viewport;
        m_ViewportStateInfo.scissorCount = 1;
        m_ViewportStateInfo.pScissors = &m_Scissor;
    }

    void GraphicsPipeline::Builder::RasterizeState(const VkPolygonMode polygonMode,
                                                   const VkCullModeFlagBits cullMode) {
        m_RasterizationInfo = {};
        m_RasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        m_RasterizationInfo.depthClampEnable = VK_FALSE;
        m_RasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
        m_RasterizationInfo.polygonMode = polygonMode;
        m_RasterizationInfo.lineWidth = 1.0f;
        m_RasterizationInfo.cullMode = cullMode;
        m_RasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
        m_RasterizationInfo.depthBiasEnable = VK_FALSE;
        m_RasterizationInfo.depthBiasConstantFactor = 0.0f;
        m_RasterizationInfo.depthBiasClamp = 0.0f;
        m_RasterizationInfo.depthBiasSlopeFactor = 0.0f;
    }

    void GraphicsPipeline::Builder::MultisampleState() {
        m_MultisamplingInfo = {};
        m_MultisamplingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        m_MultisamplingInfo.sampleShadingEnable = VK_FALSE;
        m_MultisamplingInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        m_MultisamplingInfo.minSampleShading = 1.0f;
        m_MultisamplingInfo.pSampleMask = nullptr;
        m_MultisamplingInfo.alphaToCoverageEnable = VK_FALSE;
        m_MultisamplingInfo.alphaToOneEnable = VK_FALSE;
    }

    void GraphicsPipeline::Builder::ColorBlendState(const bool blendEnabled, const VkBlendOp colorBlendingOp,
                                                    const VkBlendOp alphaBlendingOp,
                                                    const bool logicOpEnabled, const VkLogicOp logicOp) {
        m_ColorBlendAttachment = {};
        m_ColorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
            VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        m_ColorBlendAttachment.blendEnable = blendEnabled;
        m_ColorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
        m_ColorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
        m_ColorBlendAttachment.colorBlendOp = colorBlendingOp;
        m_ColorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        m_ColorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        m_ColorBlendAttachment.alphaBlendOp = alphaBlendingOp;

        m_ColorBlendingInfo = {};
        m_ColorBlendingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        m_ColorBlendingInfo.logicOpEnable = logicOpEnabled;
        m_ColorBlendingInfo.logicOp = logicOp;
        m_ColorBlendingInfo.attachmentCount = 1;
        m_ColorBlendingInfo.pAttachments = &m_ColorBlendAttachment;
        m_ColorBlendingInfo.blendConstants[0] = 0.0f;
        m_ColorBlendingInfo.blendConstants[1] = 0.0f;
        m_ColorBlendingInfo.blendConstants[2] = 0.0f;
        m_ColorBlendingInfo.blendConstants[3] = 0.0f;
    }

    void GraphicsPipeline::Builder::PipelineLayout(const std::vector<VkDescriptorSetLayout>& layouts,
                                                        const std::vector<VkPushConstantRange>&
                                                        pushConstantRanges) {
        m_PipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        m_PipelineLayoutCreateInfo.setLayoutCount = static_cast<u32>(layouts.size());
        m_PipelineLayoutCreateInfo.pSetLayouts = layouts.data();
        m_PipelineLayoutCreateInfo.pushConstantRangeCount = static_cast<u32>(pushConstantRanges.size());
        m_PipelineLayoutCreateInfo.pPushConstantRanges = pushConstantRanges.data();
    }

    std::unique_ptr<GraphicsPipeline> GraphicsPipeline::Builder::Build(const RenderPass& renderPass) const {
        VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
        if (vkCreatePipelineLayout(m_Device->GetNativeDevice(), &m_PipelineLayoutCreateInfo, nullptr,
                                   &pipelineLayout) != VK_SUCCESS) {
            Log::EngineFatal({0x01, 0x0C}, "Failed to create pipeline layout.");
        }

        const std::vector<VkDynamicState> dynamicStates = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };

        VkPipelineDynamicStateCreateInfo dynamicStateInfo{};
        dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicStateInfo.dynamicStateCount = static_cast<u32>(dynamicStates.size());
        dynamicStateInfo.pDynamicStates = dynamicStates.data();

        const VkPipelineShaderStageCreateInfo shaderStages[] = {
            m_VertexShaderModule->GetNativeShaderStageInfo(),
            m_FragmentShaderModule->GetNativeShaderStageInfo()
        };

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = shaderStages;
        pipelineInfo.pVertexInputState = &m_VertexInputInfo;
        pipelineInfo.pInputAssemblyState = &m_InputAssemblyInfo;
        pipelineInfo.pViewportState = &m_ViewportStateInfo;
        pipelineInfo.pRasterizationState = &m_RasterizationInfo;
        pipelineInfo.pMultisampleState = &m_MultisamplingInfo;
        pipelineInfo.pDepthStencilState = nullptr;
        pipelineInfo.pColorBlendState = &m_ColorBlendingInfo;
        pipelineInfo.pDynamicState = &dynamicStateInfo;

        pipelineInfo.layout = pipelineLayout;
        pipelineInfo.renderPass = renderPass.GetNativeRenderPass();
        pipelineInfo.subpass = 0;

        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
        pipelineInfo.basePipelineIndex = -1;

        VkPipeline pipeline = VK_NULL_HANDLE;
        Log::EngineTrace("Creating Vulkan pipeline...");
        if (vkCreateGraphicsPipelines(m_Device->GetNativeDevice(), nullptr, 1, &pipelineInfo, nullptr, &pipeline)
            != VK_SUCCESS) {
            Log::EngineFatal({0x01, 0x0D}, "Failed to create pipeline.");
        } else {
            Log::EngineTrace("Vulkan pipeline created.");
        }

        return std::make_unique<GraphicsPipeline>(m_Device, pipeline, pipelineLayout);
    }

}
