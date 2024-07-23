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

namespace Flashlight {
    struct PipelineInfos {
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
    };
    
    class GraphicsPipeline {
        VkPipeline m_Pipeline = VK_NULL_HANDLE;
        VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;

        VkDevice m_Device = VK_NULL_HANDLE;

    public:
        inline GraphicsPipeline(const VulkanWrapper::Device& device,
                                const std::filesystem::path& vertexShaderPath,
                                const std::filesystem::path& fragmentShaderPath,
                                const PipelineInfos& pipelineInfos);
        inline ~GraphicsPipeline();

        GraphicsPipeline(const GraphicsPipeline&) = delete;
        GraphicsPipeline(GraphicsPipeline&&) = delete;

        GraphicsPipeline& operator=(const GraphicsPipeline&) = delete;
        GraphicsPipeline& operator=(GraphicsPipeline&&) = delete;

        [[nodiscard]] inline VkPipeline GetNativePipeline() const;
        [[nodiscard]] inline VkPipelineLayout GetNativePipelineLayout() const;

        static void UseDefaultPipelineInfos(PipelineInfos& pipelineInfos);

    private:
        void Create(const std::filesystem::path& vertexShaderPath, const std::filesystem::path& fragmentShaderPath,
                    const PipelineInfos& pipelineInfos);
        inline void Destroy() const;
    };

#include "GraphicsPipeline.inl"
}
