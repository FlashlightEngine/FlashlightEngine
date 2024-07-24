/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : GraphicsPipeline.cpp
 * Description : Definitions of methods from the GraphicsPipeline class.
 */
#include "FlashlightEngine/Renderer/VulkanWrapper/GraphicsPipeline.hpp"

#include "FlashlightEngine/Renderer/VulkanWrapper/ShaderModule.hpp"

namespace Flashlight::VulkanWrapper {
    GraphicsPipeline::GraphicsPipeline(Device& device, const PipelineInfos& pipelineInfos) : m_Device(device) {
        const auto vertShaderModule = std::make_unique<ShaderModule>(m_Device.GetNativeDevice(),
                                                                     pipelineInfos.VertexShaderPath,
                                                                     ShaderType::Vertex);
        const auto fragShaderModule = std::make_unique<ShaderModule>(m_Device.GetNativeDevice(),
                                                                     pipelineInfos.FragmentShaderPath,
                                                                     ShaderType::Fragment);

        const VkPipelineShaderStageCreateInfo shaderStages[] = {
            vertShaderModule->GetNativeShaderStageInfo(),
            fragShaderModule->GetNativeShaderStageInfo()
        };

        if (vkCreatePipelineLayout(m_Device.GetNativeDevice(), &pipelineInfos.PipelineLayoutInfo, nullptr,
                                   &m_PipelineLayout) != VK_SUCCESS) {
            Log::EngineError("Failed to create pipeline layout.");
        }

        VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
        pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineCreateInfo.stageCount = 2;
        pipelineCreateInfo.pStages = shaderStages;

        pipelineCreateInfo.pVertexInputState = &pipelineInfos.VertexInputInfo;
        pipelineCreateInfo.pInputAssemblyState = &pipelineInfos.InputAssemblyInfo;
        pipelineCreateInfo.pViewportState = &pipelineInfos.ViewportStateInfo;
        pipelineCreateInfo.pRasterizationState = &pipelineInfos.RasterizationInfo;
        pipelineCreateInfo.pMultisampleState = &pipelineInfos.MultisamplingInfo;
        pipelineCreateInfo.pDepthStencilState = nullptr;
        pipelineCreateInfo.pColorBlendState = &pipelineInfos.ColorBlendingInfo;
        pipelineCreateInfo.pDynamicState = &pipelineInfos.DynamicStatesInfo;

        pipelineCreateInfo.layout = m_PipelineLayout;

        pipelineCreateInfo.renderPass = pipelineInfos.RenderPass;
        pipelineCreateInfo.subpass = pipelineInfos.SubpassIndex;

        pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
        pipelineCreateInfo.basePipelineIndex = -1;

        if (vkCreateGraphicsPipelines(m_Device.GetNativeDevice(), VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr,
                                      &m_Pipeline) != VK_SUCCESS) {
            Log::EngineError("Failed to create graphics pipeline.");
        }
    }

    void GraphicsPipeline::UseDefaultPipelineInfos(PipelineInfos& pipelineInfos, const VkRenderPass renderPass) {
        // -------------------------------------
        // Vertex input state info
        // Defines the layout of the vertex data
        // -------------------------------------
        pipelineInfos.VertexInputInfo = {};
        pipelineInfos.VertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        pipelineInfos.VertexInputInfo.vertexBindingDescriptionCount = 0;
        pipelineInfos.VertexInputInfo.pVertexBindingDescriptions = nullptr;
        pipelineInfos.VertexInputInfo.vertexAttributeDescriptionCount = 0;
        pipelineInfos.VertexInputInfo.pVertexAttributeDescriptions = nullptr;

        // --------------------------------------------
        // Vertex assembly state info
        // Defines the way vertices should be assembled
        // --------------------------------------------
        pipelineInfos.InputAssemblyInfo = {};
        pipelineInfos.InputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        pipelineInfos.InputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        pipelineInfos.InputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

        // ----------------------------------------------------------
        // Dynamic states
        // States that can be changed without recreating the pipeline
        // ----------------------------------------------------------
        pipelineInfos.DynamicStates = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
        pipelineInfos.DynamicStatesInfo = {};
        pipelineInfos.DynamicStatesInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        pipelineInfos.DynamicStatesInfo.dynamicStateCount = static_cast<u32>(pipelineInfos.DynamicStates.size());
        pipelineInfos.DynamicStatesInfo.pDynamicStates = pipelineInfos.DynamicStates.data();

        // --------------------------------------------------------------------------
        // Viewport & scissors state
        // The viewport defines the transformation from the image to the framebuffer.
        // The scissors define the actual area where the pixels will be stored.
        // --------------------------------------------------------------------------
        pipelineInfos.ViewportStateInfo = {};
        pipelineInfos.ViewportStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        pipelineInfos.ViewportStateInfo.viewportCount = 1;
        pipelineInfos.ViewportStateInfo.scissorCount = 1;

        // ----------------------------------------------------------------------------------------
        // Rasterization
        // The rasterization is the process where the pixels (fragments) covered by geometry will be evaluated.
        // ----------------------------------------------------------------------------------------
        pipelineInfos.RasterizationInfo = {};
        pipelineInfos.RasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        pipelineInfos.RasterizationInfo.depthClampEnable = VK_FALSE;
        pipelineInfos.RasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
        pipelineInfos.RasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
        pipelineInfos.RasterizationInfo.lineWidth = 1.0f;
        pipelineInfos.RasterizationInfo.cullMode = VK_CULL_MODE_BACK_BIT;
        pipelineInfos.RasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
        pipelineInfos.RasterizationInfo.depthBiasEnable = VK_FALSE;
        pipelineInfos.RasterizationInfo.depthBiasConstantFactor = 0.0f;
        pipelineInfos.RasterizationInfo.depthBiasClamp = 0.0f;
        pipelineInfos.RasterizationInfo.depthBiasSlopeFactor = 0.0f;

        // -----------------------------------------
        // Multisampling
        // Multisampling is a form of anti-aliasing.
        // -----------------------------------------
        pipelineInfos.MultisamplingInfo = {};
        pipelineInfos.MultisamplingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        pipelineInfos.MultisamplingInfo.sampleShadingEnable = VK_FALSE;
        pipelineInfos.MultisamplingInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        pipelineInfos.MultisamplingInfo.minSampleShading = 1.0f;
        pipelineInfos.MultisamplingInfo.pSampleMask = nullptr;
        pipelineInfos.MultisamplingInfo.alphaToCoverageEnable = VK_FALSE;
        pipelineInfos.MultisamplingInfo.alphaToOneEnable = VK_FALSE;

        // --------------------------------------------------------------------------------------------------------
        // Color blending
        // The color blending is the stage where a fragment color is combined with the color already present in the
        // framebuffer.
        // --------------------------------------------------------------------------------------------------------
        pipelineInfos.ColorBlendAttachment = {};
        pipelineInfos.ColorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
            VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        pipelineInfos.ColorBlendAttachment.blendEnable = VK_FALSE;
        pipelineInfos.ColorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
        pipelineInfos.ColorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
        pipelineInfos.ColorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        pipelineInfos.ColorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        pipelineInfos.ColorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        pipelineInfos.ColorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

        pipelineInfos.ColorBlendingInfo = {};
        pipelineInfos.ColorBlendingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        pipelineInfos.ColorBlendingInfo.logicOpEnable = VK_FALSE;
        pipelineInfos.ColorBlendingInfo.logicOp = VK_LOGIC_OP_COPY;
        pipelineInfos.ColorBlendingInfo.attachmentCount = 1;
        pipelineInfos.ColorBlendingInfo.pAttachments = &pipelineInfos.ColorBlendAttachment;
        pipelineInfos.ColorBlendingInfo.blendConstants[0] = 0.0f;
        pipelineInfos.ColorBlendingInfo.blendConstants[1] = 0.0f;
        pipelineInfos.ColorBlendingInfo.blendConstants[2] = 0.0f;
        pipelineInfos.ColorBlendingInfo.blendConstants[3] = 0.0f;

        // -----------------------------------------------------------------------
        // Pipeline layout
        // The pipeline layout lets us use uniforms and push constants in shaders.
        // -----------------------------------------------------------------------
        pipelineInfos.PipelineLayoutInfo = {};
        pipelineInfos.PipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineInfos.PipelineLayoutInfo.setLayoutCount = 0;
        pipelineInfos.PipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineInfos.PipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineInfos.PipelineLayoutInfo.pPushConstantRanges = nullptr;

        // -----------
        // Render pass
        // -----------
        pipelineInfos.RenderPass = renderPass;
        pipelineInfos.SubpassIndex = 0;
    }
}
