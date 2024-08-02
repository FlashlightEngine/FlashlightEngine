/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : VulkanPipelineUtils.cpp
 * Description : Definitions of methods from VulkanPipelineUtils.hpp.
 */
#include <FlashlightEngine/VulkanRenderer/VulkanInitializers.hpp>
#include <FlashlightEngine/VulkanRenderer/VulkanUtils/VulkanPipelineUtils.hpp>

#include <fstream>

namespace Flashlight::VulkanRenderer::VulkanUtils {
    void CreateShaderModule(const VkDevice device, const std::filesystem::path& filePath,
                            const ShaderType& shaderType, VkShaderModule* outShaderModule) {
        const std::string shaderSourceCode = ReadFile(filePath);

#if defined(FL_DEBUG)
        constexpr bool optimizeShader = false;
#else
        constexpr bool optimizeShader = true;
#endif

        const auto compiledShader = CompileShaderCode(shaderSourceCode, shaderType, filePath.filename().string(),
                                                      optimizeShader);

        // Create a new shader module using the compiled shader.
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.pNext = nullptr;

        createInfo.codeSize = compiledShader.size() * sizeof(u32);
        createInfo.pCode = compiledShader.data();

        VkShaderModule shaderModule;
        VK_CHECK(vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule))

        *outShaderModule = shaderModule;
    }

    std::string ReadFile(const std::filesystem::path& path) {
        std::ifstream file(path, std::ios::ate);

        if (!file.is_open()) {
            Log::EngineError("Failed to open shader file at path {}.", path.string());
        }

        const size fileSize = file.tellg();
        std::vector<char> buffer(fileSize / sizeof(char));

        file.seekg(0);
        file.read(buffer.data(), static_cast<std::streamsize>(fileSize));

        file.close();

        std::string result;

        // Remove null characters if there is any.
        std::ranges::copy_if(buffer, std::back_inserter(result), [](const char c) -> bool {
            return c != '\0';
        });

        return result;
    }

    std::vector<u32> CompileShaderCode(const std::string& code, const ShaderType& shaderType,
                                       const std::string& sourceFileName, const bool optimize) {
        const shaderc::Compiler compiler;
        shaderc::CompileOptions options;

        if (optimize) {
            options.SetOptimizationLevel(shaderc_optimization_level_size);
        }

        options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_3);

        const auto spirvCompilationResult = compiler.CompileGlslToSpv(
            code, static_cast<shaderc_shader_kind>(shaderType), sourceFileName.c_str(), options);
        if (spirvCompilationResult.GetCompilationStatus() != shaderc_compilation_status_success) {
            Log::EngineError("Failed to compile shader to SPIR-V.\n Error: {}",
                             spirvCompilationResult.GetErrorMessage());
        }

        return {spirvCompilationResult.cbegin(), spirvCompilationResult.cend()};
    }

    void PipelineBuilder::Clear() {
        m_InputAssembly = {.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};

        m_Rasterizer = {.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};

        m_ColorBlendAttachment = {};

        m_Multisampling = {.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO};

        m_PipelineLayout = {};

        m_DepthStencil = {.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO};

        m_RenderInfo = {.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO};

        m_ShaderStages.clear();
    }

    void PipelineBuilder::SetShaders(const VkShaderModule vertexShader, const VkShaderModule fragmentShader) {
        m_ShaderStages.clear();

        m_ShaderStages.push_back(
            VulkanInit::PipelineShaderStageCreateInfo(VK_SHADER_STAGE_VERTEX_BIT, vertexShader));
        m_ShaderStages.push_back(
            VulkanInit::PipelineShaderStageCreateInfo(VK_SHADER_STAGE_FRAGMENT_BIT, fragmentShader));
    }

    void PipelineBuilder::SetInputTopology(const VkPrimitiveTopology topology) {
        m_InputAssembly.topology = topology;
        m_InputAssembly.primitiveRestartEnable = VK_FALSE;
    }

    void PipelineBuilder::SetPolygonMode(const VkPolygonMode mode) {
        m_Rasterizer.polygonMode = mode;
        m_Rasterizer.lineWidth = 1.0f;
    }

    void PipelineBuilder::SetCullMode(const VkCullModeFlags cullMode, const VkFrontFace frontFace) {
        m_Rasterizer.cullMode = cullMode;
        m_Rasterizer.frontFace = frontFace;
    }

    void PipelineBuilder::SetMultisamplingNone() {
        m_Multisampling.sampleShadingEnable = VK_FALSE;
        m_Multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        m_Multisampling.minSampleShading = 1.0f;
        m_Multisampling.pSampleMask = nullptr;
        m_Multisampling.alphaToCoverageEnable = VK_FALSE;
        m_Multisampling.alphaToOneEnable = VK_FALSE;
    }

    void PipelineBuilder::DisableBlending() {
        m_ColorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
            VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        m_ColorBlendAttachment.blendEnable = VK_FALSE;
    }

    void PipelineBuilder::EnableAdditiveBlending() {
        m_ColorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
            VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        m_ColorBlendAttachment.blendEnable = VK_TRUE;
        m_ColorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        m_ColorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE;
        m_ColorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        m_ColorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        m_ColorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        m_ColorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
    }

    void PipelineBuilder::EnableBlendingAlphaBlend() {
        m_ColorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
            VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        m_ColorBlendAttachment.blendEnable = VK_TRUE;
        m_ColorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        m_ColorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        m_ColorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        m_ColorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        m_ColorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        m_ColorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
    }

    void PipelineBuilder::SetColorAttachmentFormat(const VkFormat format) {
        m_ColorAttachmentFormat = format;
    }

    void PipelineBuilder::SetDepthFormat(const VkFormat format) {
        m_DepthAttachmentFormat = format;
    }

    void PipelineBuilder::DisableDepthTest() {
        m_DepthStencil.depthTestEnable = VK_FALSE;
        m_DepthStencil.depthWriteEnable = VK_FALSE;
        m_DepthStencil.depthCompareOp = VK_COMPARE_OP_NEVER;
        m_DepthStencil.depthBoundsTestEnable = VK_FALSE;
        m_DepthStencil.stencilTestEnable = VK_FALSE;
        m_DepthStencil.front = {};
        m_DepthStencil.back = {};
        m_DepthStencil.minDepthBounds = 0.0f;
        m_DepthStencil.maxDepthBounds = 1.0f;
    }

    void PipelineBuilder::EnableDepthTest(const bool depthWriteEnable, const VkCompareOp op) {
        m_DepthStencil.depthTestEnable = VK_TRUE;
        m_DepthStencil.depthWriteEnable = depthWriteEnable;
        m_DepthStencil.depthCompareOp = op;
        m_DepthStencil.depthBoundsTestEnable = VK_FALSE;
        m_DepthStencil.stencilTestEnable = VK_FALSE;
        m_DepthStencil.front = {};
        m_DepthStencil.back = {};
        m_DepthStencil.minDepthBounds = 0.0f;
        m_DepthStencil.maxDepthBounds = 1.0f;
    }

    void PipelineBuilder::SetPipelineLayout(const VkPipelineLayout layout) {
        m_PipelineLayout = layout;
    }

    VkPipeline PipelineBuilder::BuildPipeline(const VkDevice device) {
        // Make viewport state from the stored viewport and scissor.
        // At the moment we won't support multiple viewports or scissors.
        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.pNext = nullptr;

        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;

        // Setup dummy color blending. We aren't using transparent objects yet.
        // The blending is just "no blend", but we do write to the color attachment.
        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.pNext = nullptr;

        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &m_ColorBlendAttachment;

        // Completely clear VkVertexInputStateCreateInfo, no need for it.
        constexpr VkPipelineVertexInputStateCreateInfo vertexInputInfo = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO
        };

        VkGraphicsPipelineCreateInfo pipelineInfo = {.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};

        m_RenderInfo.colorAttachmentCount = 1;
        m_RenderInfo.pColorAttachmentFormats = &m_ColorAttachmentFormat;
        m_RenderInfo.depthAttachmentFormat = m_DepthAttachmentFormat; // For some reason this works.

        pipelineInfo.pNext = &m_RenderInfo;

        pipelineInfo.stageCount = static_cast<u32>(m_ShaderStages.size());
        pipelineInfo.pStages = m_ShaderStages.data();
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &m_InputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &m_Rasterizer;
        pipelineInfo.pMultisampleState = &m_Multisampling;
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDepthStencilState = &m_DepthStencil;
        pipelineInfo.layout = m_PipelineLayout;

        constexpr VkDynamicState dynamicStates[] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

        VkPipelineDynamicStateCreateInfo dynamicStatesInfo = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO
        };
        dynamicStatesInfo.dynamicStateCount = static_cast<u32>(std::size(dynamicStates));
        dynamicStatesInfo.pDynamicStates = dynamicStates;

        pipelineInfo.pDynamicState = &dynamicStatesInfo;

        VkPipeline newPipeline;
        if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &newPipeline) !=
            VK_SUCCESS) {
            Log::EngineError("Failed to create pipeline.");
            return VK_NULL_HANDLE;
        }

        return newPipeline;
    }
}
