#pragma once

inline void VulkanGraphicsPipelineStages::UseDefaultPipelineConfiguration(const std::vector<VkVertexInputAttributeDescription>
                                                                          &vertexAttributeDescriptions,
                                                                          const std::vector<VkVertexInputBindingDescription>
                                                                          &vertexBindingDescriptions) {
    DynamicStates.push_back(VK_DYNAMIC_STATE_VIEWPORT);
    DynamicStates.push_back(VK_DYNAMIC_STATE_SCISSOR);

    DynamicStatesInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    DynamicStatesInfo.dynamicStateCount = static_cast<u32>(DynamicStates.size());
    DynamicStatesInfo.pDynamicStates = DynamicStates.data();

    VertexInput.sType =  VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    VertexInput.vertexAttributeDescriptionCount = static_cast<u32>(vertexAttributeDescriptions.size());
    VertexInput.pVertexAttributeDescriptions = vertexAttributeDescriptions.data();
    VertexInput.vertexBindingDescriptionCount = static_cast<u32>(vertexBindingDescriptions.size());
    VertexInput.pVertexBindingDescriptions = vertexBindingDescriptions.data();

    InputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    InputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    InputAssembly.primitiveRestartEnable = VK_FALSE;

    Rasterization.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    Rasterization.depthClampEnable = VK_FALSE;

    Rasterization.rasterizerDiscardEnable = VK_FALSE;

    Rasterization.polygonMode = VK_POLYGON_MODE_FILL;

    Rasterization.lineWidth = 1.0f;

    Rasterization.cullMode = VK_CULL_MODE_BACK_BIT;
    Rasterization.frontFace = VK_FRONT_FACE_CLOCKWISE;

    Rasterization.depthBiasEnable = VK_FALSE;
    Rasterization.depthBiasConstantFactor = 0.0f;
    Rasterization.depthBiasClamp = 0.0f;
    Rasterization.depthBiasSlopeFactor = 0.0f;

    Multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    Multisampling.sampleShadingEnable = VK_FALSE;
    Multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    Multisampling.minSampleShading = 1.0f;
    Multisampling.pSampleMask = nullptr;
    Multisampling.alphaToCoverageEnable = VK_FALSE;
    Multisampling.alphaToOneEnable = VK_FALSE;

    // Everything keeps its default value.
    DepthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;

    ColorAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
                                     VK_COLOR_COMPONENT_A_BIT;
    ColorAttachment.blendEnable = VK_FALSE;
    ColorAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    ColorAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    ColorAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    ColorAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    ColorAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    ColorAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

    ColorBlend.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    ColorBlend.logicOpEnable = VK_FALSE;
    ColorBlend.logicOp = VK_LOGIC_OP_COPY;
    ColorBlend.attachmentCount = 1;
    ColorBlend.pAttachments = &ColorAttachment;
    ColorBlend.blendConstants[0] = 0.0f;
    ColorBlend.blendConstants[1] = 0.0f;
    ColorBlend.blendConstants[2] = 0.0f;
    ColorBlend.blendConstants[3] = 0.0f;

    
}
