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
    class GraphicsPipeline {
        VkPipeline m_GraphicsPipeline = VK_NULL_HANDLE;

        VkDevice m_Device = VK_NULL_HANDLE;

    public:
        inline GraphicsPipeline(const VulkanWrapper::Device& device,
                                const std::filesystem::path& vertexShaderPath,
                                const std::filesystem::path& fragmentShaderPath);
        inline ~GraphicsPipeline();

        GraphicsPipeline(const GraphicsPipeline&) = delete;
        GraphicsPipeline(GraphicsPipeline&&) = delete;

        GraphicsPipeline& operator=(const GraphicsPipeline&) = delete;
        GraphicsPipeline& operator=(GraphicsPipeline&&) = delete;

        [[nodiscard]] inline VkPipeline GetNativePipeline() const;

    private:
        void Create(const std::filesystem::path& vertexShaderPath, const std::filesystem::path& fragmentShaderPath);
        inline void Destroy() const;
    };

#include "GraphicsPipeline.inl"
}
