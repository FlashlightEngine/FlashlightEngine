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
                                  const std::filesystem::path& fragmentShaderPath) {
        const auto vertShaderModule = std::make_unique<ShaderModule>(m_Device, vertexShaderPath, ShaderType::Vertex);
        const auto fragShaderModule = std::make_unique<ShaderModule>(m_Device, fragmentShaderPath, ShaderType::Fragment);

        VkPipelineShaderStageCreateInfo shaderStages[] = {
            vertShaderModule->GetNativeShaderStageInfo(),
            fragShaderModule->GetNativeShaderStageInfo()
        };
    }
}
