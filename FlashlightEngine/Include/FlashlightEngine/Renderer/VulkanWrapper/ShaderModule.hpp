/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : ShaderModule.hpp
 * Description : Declaration of the ShaderModule class and the ShaderType enum. A shader module is a wrapper around
 *               the shader code for Vulkan.
 */
#pragma once

#include "FlashlightEngine/Renderer/VulkanWrapper/Device.hpp"

#include <shaderc/shaderc.hpp>

namespace Flashlight::VulkanWrapper {
    // Only vertex and fragment shaders supported for now. 
    enum class ShaderType : VkShaderStageFlags {
        Vertex = VK_SHADER_STAGE_VERTEX_BIT,
        Fragment = VK_SHADER_STAGE_FRAGMENT_BIT
    };

    class ShaderModule {
        VkShaderModule m_ShaderModule;
        VkPipelineShaderStageCreateInfo m_ShaderStageInfo;

        std::shared_ptr<Device> m_Device;

    public:
        ShaderModule(const std::shared_ptr<Device>& device, const std::filesystem::path& shaderPath,
                     const ShaderType& shaderType);
        inline ~ShaderModule();

        ShaderModule(const ShaderModule&) = delete;
        ShaderModule(ShaderModule&&) = delete;

        ShaderModule& operator=(const ShaderModule&) = delete;
        ShaderModule& operator=(ShaderModule&&) = delete;

        [[nodiscard]] inline VkShaderModule GetNativeShaderModule() const;
        [[nodiscard]] inline VkPipelineShaderStageCreateInfo GetNativeShaderStageInfo() const;

    private:
        [[nodiscard]] static std::string ReadFile(const std::filesystem::path& filePath);
        [[nodiscard]] static std::vector<u32> CompileGlslToSpv(const std::string& code,
                                                               const shaderc_shader_kind& shaderType,
                                                               const std::string& sourceFileName,
                                                               bool optimize = true);
        void CreateShaderModule(const std::vector<u32>& spvBytecode);
        void CreateShaderStage(const ShaderType& shaderType);
    };

#include "ShaderModule.inl"
}
