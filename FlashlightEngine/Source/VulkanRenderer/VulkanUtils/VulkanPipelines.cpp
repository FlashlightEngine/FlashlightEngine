/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : VulkanPipelines.cpp
 * Description : Definitions of methods from VulkanPipelines.hpp.
 */
#include <FlashlightEngine/VulkanRenderer/VulkanInitializers.hpp>
#include <FlashlightEngine/VulkanRenderer/VulkanUtils/VulkanPipelines.hpp>

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
                                       const std::string& sourceFileName, bool optimize) {
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
}
