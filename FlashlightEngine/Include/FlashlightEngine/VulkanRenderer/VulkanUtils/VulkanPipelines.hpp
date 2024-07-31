/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : VulkanPipelines.hpp
 * Description : Abstraction of Vulkan pipelines.
 */
#pragma once

#include <FlashlightEngine/VulkanRenderer/VulkanTypes.hpp>

#include <shaderc/shaderc.hpp>

namespace Flashlight::VulkanRenderer::VulkanUtils {
    enum class ShaderType {
        Compute = shaderc_glsl_compute_shader
    };

    void CreateShaderModule(VkDevice device, const std::filesystem::path& filePath, const ShaderType& shaderType,
                            VkShaderModule* outShaderModule);

    std::string ReadFile(const std::filesystem::path& path);
    std::vector<u32> CompileShaderCode(const std::string& code, const ShaderType& shaderType,
                                       const std::string& sourceFileName, bool optimize);
}
