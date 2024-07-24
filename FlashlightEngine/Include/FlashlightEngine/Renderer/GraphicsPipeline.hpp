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
#include "FlashlightEngine/Renderer/RenderPass.hpp"

namespace Flashlight {
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

        VkDevice m_Device = VK_NULL_HANDLE;

    public:
        inline GraphicsPipeline(const VulkanWrapper::Device& device, const PipelineInfos& pipelineInfos);
        inline ~GraphicsPipeline();

        GraphicsPipeline(const GraphicsPipeline&) = delete;
        GraphicsPipeline(GraphicsPipeline&&) = delete;

        GraphicsPipeline& operator=(const GraphicsPipeline&) = delete;
        GraphicsPipeline& operator=(GraphicsPipeline&&) = delete;

        [[nodiscard]] inline VkPipeline GetNativePipeline() const;
        [[nodiscard]] inline VkPipelineLayout GetNativePipelineLayout() const;

        static void UseDefaultPipelineInfos(PipelineInfos& pipelineInfos, const RenderPass& renderPass);

    private:
        void Create(const PipelineInfos& pipelineInfos);
        inline void Destroy() const;
    };

#include "GraphicsPipeline.inl"
}
