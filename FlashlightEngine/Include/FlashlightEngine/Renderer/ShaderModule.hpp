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

namespace Flashlight {
    // Only vertex and fragment shaders supported for now. 
    enum class ShaderType {
        Vertex = shaderc_vertex_shader,
        Fragment = shaderc_fragment_shader
    };

    class ShaderModule {
        VkShaderModule m_ShaderModule;

        VkDevice m_Device = VK_NULL_HANDLE;

    public:
        inline ShaderModule(VkDevice device, const std::filesystem::path& shaderPath,
                            const ShaderType& shaderType, bool optimizeShaders = false);
        inline ~ShaderModule();

        ShaderModule(const ShaderModule&) = delete;
        ShaderModule(ShaderModule&&) = delete;

        ShaderModule& operator=(const ShaderModule&) = delete;
        ShaderModule& operator=(ShaderModule&&) = delete;

        [[nodiscard]] inline VkShaderModule GetNativeShaderModule() const;

    private:
        void Create(const std::filesystem::path& shaderPath, const ShaderType& shaderType,
                    bool optimizeShaders = false);
        inline void Destroy() const;

        [[nodiscard]] static std::string ReadShaderFile(const std::filesystem::path& shaderPath);
        [[nodiscard]] static std::vector<u32> CompileShader(const std::string& sourceName,
                                                            const ShaderType& shaderType,
                                                            const std::string& source, bool optimize = false);
    };

#include "ShaderModule.inl"
}
