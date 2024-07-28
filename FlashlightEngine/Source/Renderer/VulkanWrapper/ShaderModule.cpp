/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : ShaderModule.cpp
 * Description : Definitions of methods from the ShaderModule class.
 */
#include "FlashlightEngine/Renderer/VulkanWrapper/ShaderModule.hpp"

namespace Flashlight::VulkanWrapper {
        ShaderModule::ShaderModule(const std::shared_ptr<Device>& device,
                                      const std::filesystem::path& shaderPath,
                                      const ShaderType& shaderType) : m_Device(device) {
        const auto shaderSourceCode = ReadFile(shaderPath);

#if defined(FL_DEBUG)
        constexpr bool optimizeShaders = true;
#else
        constexpr bool optimizeShaders = false;
#endif

        shaderc_shader_kind kind;
        if (shaderType == ShaderType::Vertex) {
            kind = shaderc_glsl_vertex_shader;
        } else if (shaderType == ShaderType::Fragment) {
            kind = shaderc_glsl_fragment_shader;
        }

        const auto shaderSpirv = CompileGlslToSpv(shaderSourceCode, kind, shaderPath.filename().string(), optimizeShaders);
        CreateShaderModule(shaderSpirv);
        CreateShaderStage(shaderType);
    }

    std::string ShaderModule::ReadFile(const std::filesystem::path& filePath) {
        std::ifstream file(filePath, std::ios::ate);

        if (!file.is_open()) {
            Log::EngineError("Failed to open file at path {0}", filePath.string());
        }

        const size fileSize = file.tellg();
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), static_cast<std::streamsize>(fileSize));

        file.close();

        std::string result;

        std::ranges::copy_if(buffer, std::back_inserter(result), [](const char c) -> bool {
            return c != '\0';
        });

        return result;
    }

    std::vector<u32> ShaderModule::CompileGlslToSpv(const std::string& code, const shaderc_shader_kind& shaderType,
                                                    const std::string& sourceFileName, const bool optimize) {
        const shaderc::Compiler compiler;
        shaderc::CompileOptions options;
        if (optimize) {
            options.SetOptimizationLevel(shaderc_optimization_level_size);
        }

        options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_0);

        const auto spirvCompilationResult = compiler.CompileGlslToSpv(
            code, shaderType, sourceFileName.c_str(), options);
        if (spirvCompilationResult.GetCompilationStatus() != shaderc_compilation_status_success) {
            Log::EngineError("Failed to compile GLSL to SPIR-V.\n Error: {0}",
                             spirvCompilationResult.GetErrorMessage());
        }

        return {spirvCompilationResult.cbegin(), spirvCompilationResult.cend()};
    }

    void ShaderModule::CreateShaderModule(const std::vector<u32>& spvBytecode) {
        VkShaderModuleCreateInfo shaderModuleCreateInfo{};
        shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        shaderModuleCreateInfo.codeSize = spvBytecode.size() * sizeof(u32);
        shaderModuleCreateInfo.pCode = spvBytecode.data();

        Log::EngineTrace("Creating Vulkan shader module...");
        if (vkCreateShaderModule(m_Device->GetNativeDevice(), &shaderModuleCreateInfo, nullptr, &m_ShaderModule)
            != VK_SUCCESS) {
            Log::EngineFatal({0x01, 0x0B}, "Failed to create shader module.");
        } else {
            Log::EngineTrace("Vulkan shader module created.");
        }
    }

    void ShaderModule::CreateShaderStage(const ShaderType& shaderType) {
        VkPipelineShaderStageCreateInfo shaderStageInfo{};
        shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStageInfo.stage = static_cast<VkShaderStageFlagBits>(shaderType);
        shaderStageInfo.module = m_ShaderModule;
        shaderStageInfo.pName = "main";

        m_ShaderStageInfo = shaderStageInfo;
    }
}
