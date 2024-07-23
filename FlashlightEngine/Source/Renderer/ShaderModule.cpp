/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : ShaderModule.cpp
 * Description : Definitions of methods from the ShaderModule class.
 */
#include "FlashlightEngine/Renderer/ShaderModule.hpp"

namespace Flashlight {
    void ShaderModule::Create(const std::filesystem::path& shaderPath, const ShaderType& shaderType,
                              const bool optimizeShaders) {
        const std::string shaderSource = ReadShaderFile(shaderPath);
        const std::vector<u32> shaderBytecode = CompileShader(shaderPath.filename().string(), shaderType,
                                                              shaderSource,
                                                              optimizeShaders);

        VkShaderModuleCreateInfo shaderModuleCreateInfo{};
        shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        shaderModuleCreateInfo.codeSize = shaderBytecode.size();
        shaderModuleCreateInfo.pCode = shaderBytecode.data();

        if (vkCreateShaderModule(m_Device, &shaderModuleCreateInfo, nullptr, &m_ShaderModule) != VK_SUCCESS) {
            Log::EngineError("Failed to create shader module.");
        }
    }

    std::string ShaderModule::ReadShaderFile(const std::filesystem::path& shaderPath) {
        std::ifstream shaderFile(shaderPath, std::ios::ate);

        if (!shaderFile.is_open()) {
            Log::EngineError("Failed to open file at path: {0}", shaderPath);
        }

        const size fileSize = shaderFile.tellg();
        std::vector<char> buffer(fileSize);

        shaderFile.seekg(0);
        shaderFile.read(buffer.data(), static_cast<std::streamsize>(fileSize));

        shaderFile.close();

        std::string result;

        std::ranges::copy_if(buffer, std::back_inserter(result), [](const char c) -> bool {
            return c != '\0';
        });

        return result;
    }

    std::vector<u32> ShaderModule::CompileShader(const std::string& sourceName, const ShaderType& shaderType,
                                                 const std::string& source, const bool optimize) {
        const shaderc::Compiler compiler;
        shaderc::CompileOptions options;

        if (optimize) {
            options.SetOptimizationLevel(shaderc_optimization_level_size);
        }

        shaderc::SpvCompilationResult module =
            compiler.CompileGlslToSpv(source, static_cast<shaderc_shader_kind>(shaderType), sourceName.c_str(),
                                      options);

        if (module.GetCompilationStatus() != shaderc_compilation_status_success) {
            Log::EngineError("Failed to compile shader: {0}", module.GetErrorMessage());
            return {}; // Return an empty vector.
        }

        return {module.cbegin(), module.cend()};
    }

}
