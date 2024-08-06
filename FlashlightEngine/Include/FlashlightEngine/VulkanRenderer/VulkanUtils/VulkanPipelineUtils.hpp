/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : VulkanPipelineUtils.hpp
 * Description : Abstraction and utilities for Vulkan pipelines.
 */
#pragma once

#include <FlashlightEngine/VulkanRenderer/VulkanTypes.hpp>

#include <filesystem>
#include <fstream>

namespace Flashlight::Renderer::VulkanUtils {

    [[nodiscard]] bool CreateShaderModule(VkDevice device, const std::filesystem::path& filePath,
                                          VkShaderModule* outShaderModule);

    class PipelineBuilder {
        std::vector<VkPipelineShaderStageCreateInfo> m_ShaderStages;

        VkPipelineInputAssemblyStateCreateInfo m_InputAssembly;
        VkPipelineRasterizationStateCreateInfo m_Rasterizer;
        VkPipelineColorBlendAttachmentState m_ColorBlendAttachment;
        VkPipelineMultisampleStateCreateInfo m_Multisampling;
        VkPipelineLayout m_PipelineLayout;
        VkPipelineDepthStencilStateCreateInfo m_DepthStencil;
        VkPipelineRenderingCreateInfo m_RenderInfo;
        VkFormat m_ColorAttachmentFormat;
        VkFormat m_DepthAttachmentFormat;

    public:
        PipelineBuilder() {
            Clear();
        }

        void Clear();

        void SetShaders(VkShaderModule vertexShader, VkShaderModule fragmentShader);
        void SetInputTopology(VkPrimitiveTopology topology);
        void SetPolygonMode(VkPolygonMode mode);
        void SetCullMode(VkCullModeFlags cullMode, VkFrontFace frontFace);
        void SetMultisamplingNone();
        void DisableBlending();
        void EnableAdditiveBlending();
        void EnableBlendingAlphaBlend();
        void SetColorAttachmentFormat(VkFormat format);
        void SetDepthFormat(VkFormat format);
        void DisableDepthTest();
        void EnableDepthTest(bool depthWriteEnable, VkCompareOp op);
        void SetPipelineLayout(VkPipelineLayout layout);

        VkPipeline BuildPipeline(VkDevice device);
    };
}
