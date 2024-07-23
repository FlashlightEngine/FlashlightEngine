/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : GraphicsPipeline.cpp
 * Description : Definitions of methods from the GraphicsPipeline class.
 */
#include "FlashlightEngine/Renderer/GraphicsPipeline.hpp"

#include "FlashlightEngine/Renderer/ShaderModule.hpp"

namespace Flashlight {
    void GraphicsPipeline::Create(const std::filesystem::path& vertexShaderPath,
                                  const std::filesystem::path& fragmentShaderPath,
                                  const PipelineInfos& pipelineInfos) {
        const auto vertShaderModule = std::make_unique<
            ShaderModule>(m_Device, vertexShaderPath, ShaderType::Vertex);
        const auto fragShaderModule = std::make_unique<ShaderModule>(m_Device, fragmentShaderPath,
                                                                     ShaderType::Fragment);

        VkPipelineShaderStageCreateInfo shaderStages[] = {
            vertShaderModule->GetNativeShaderStageInfo(),
            fragShaderModule->GetNativeShaderStageInfo()
        };

        if (vkCreatePipelineLayout(m_Device, &pipelineInfos.PipelineLayoutInfo, nullptr, &m_PipelineLayout)
            != VK_SUCCESS) {
            Log::EngineError("Failed to create pipeline layout.");
        }
    }

    void GraphicsPipeline::UseDefaultPipelineInfos(PipelineInfos& pipelineInfos) {
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
    }
}
