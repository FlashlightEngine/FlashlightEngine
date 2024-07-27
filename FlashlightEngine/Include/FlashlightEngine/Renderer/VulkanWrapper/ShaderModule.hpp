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
        inline ShaderModule(const std::shared_ptr<Device>& device, const std::filesystem::path& shaderPath, const ShaderType& shaderType);
        inline ~ShaderModule();

        ShaderModule(const ShaderModule&) = delete;
        ShaderModule(ShaderModule&&) = delete;

        ShaderModule& operator=(const ShaderModule&) = delete;
        ShaderModule& operator=(ShaderModule&&) = delete;

        [[nodiscard]] inline VkShaderModule GetNativeShaderModule() const;
        [[nodiscard]] inline VkPipelineShaderStageCreateInfo GetNativeShaderStageInfo() const;

    private:
        void CreateShaderModule(const std::filesystem::path& shaderPath);
        void CreateShaderStage(const ShaderType& shaderType);

        [[nodiscard]] static std::vector<char> ReadShaderFile(const std::filesystem::path& shaderPath);
    };

#include "ShaderModule.inl"
}
