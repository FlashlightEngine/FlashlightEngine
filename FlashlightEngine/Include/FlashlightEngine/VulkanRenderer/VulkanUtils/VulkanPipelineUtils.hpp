/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : VulkanPipelineUtils.hpp
 * Description : Abstraction and utilities for Vulkan pipelines.
 */
#pragma once

#include <FlashlightEngine/VulkanRenderer/VulkanTypes.hpp>

#include <shaderc/shaderc.hpp>

namespace Flashlight::VulkanRenderer::VulkanUtils {
    enum class ShaderType {
        Vertex = shaderc_glsl_vertex_shader,
        Fragment = shaderc_glsl_fragment_shader,
        Compute = shaderc_glsl_compute_shader
    };

    void CreateShaderModule(VkDevice device, const std::filesystem::path& filePath, const ShaderType& shaderType,
                            VkShaderModule* outShaderModule);

    std::string ReadFile(const std::filesystem::path& path);
    std::vector<u32> CompileShaderCode(const std::string& code, const ShaderType& shaderType,
                                       const std::string& sourceFileName, bool optimize);

    class PipelineBuilder {
        std::vector<VkPipelineShaderStageCreateInfo> m_ShaderStages;

        VkPipelineInputAssemblyStateCreateInfo m_InputAssembly;
        VkPipelineRasterizationStateCreateInfo m_Rasterizer;
        VkPipelineColorBlendAttachmentState    m_ColorBlendAttachment;
        VkPipelineMultisampleStateCreateInfo   m_Multisampling;
        VkPipelineLayout                       m_PipelineLayout;
        VkPipelineDepthStencilStateCreateInfo  m_DepthStencil;
        VkPipelineRenderingCreateInfo          m_RenderInfo;
        VkFormat                               m_ColorAttachmentFormat;
        VkFormat                               m_DepthAttachmentFormat;

    public:
        PipelineBuilder() { Clear(); }
        
        void Clear();

        void SetShaders(VkShaderModule vertexShader, VkShaderModule fragmentShader);
        void SetInputTopology(VkPrimitiveTopology topology);
        void SetPolygonMode(VkPolygonMode mode);
        void SetCullMode(VkCullModeFlags cullMode, VkFrontFace frontFace);
        void SetMultisamplingNone();
        void DisableBlending();
        void SetColorAttachmentFormat(VkFormat format);
        void SetDepthFormat(VkFormat format);
        void DisableDepthTest();
        void EnableDepthTest(bool depthWriteEnable, VkCompareOp op);
        void SetPipelineLayout(VkPipelineLayout layout);
        
        VkPipeline BuildPipeline(VkDevice device);
    };
}
