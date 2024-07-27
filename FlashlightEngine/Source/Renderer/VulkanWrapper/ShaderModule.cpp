/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : ShaderModule.cpp
 * Description : Definitions of methods from the ShaderModule class.
 */
#include "FlashlightEngine/Renderer/VulkanWrapper/ShaderModule.hpp"

namespace Flashlight::VulkanWrapper {
    void ShaderModule::CreateShaderModule(const std::filesystem::path& shaderPath) {
        const std::vector<char> shaderSource = ReadShaderFile(shaderPath);

        VkShaderModuleCreateInfo shaderModuleCreateInfo{};
        shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        shaderModuleCreateInfo.codeSize = shaderSource.size();
        shaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(shaderSource.data());

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

    std::vector<char> ShaderModule::ReadShaderFile(const std::filesystem::path& shaderPath) {
        std::string path{shaderPath.string()};
        path.append(".spv");

        std::ifstream shaderFile(path, std::ios::ate | std::ios::binary);

        if (!shaderFile.is_open()) {
            Log::EngineError("Failed to open file at path: {0}", shaderPath.string());
        }

        const size fileSize = shaderFile.tellg();
        std::vector<char> buffer(fileSize);

        shaderFile.seekg(0);
        shaderFile.read(buffer.data(), static_cast<std::streamsize>(fileSize));

        shaderFile.close();


        return buffer;
    }
}
