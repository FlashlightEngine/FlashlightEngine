#pragma once

#include "VulkanShaderModule.hpp"

#include <array>

namespace Flashlight {

namespace VulkanWrapper {

    enum class GraphicsShaderType {
        VertexShader,
        FragmentShader
    };
    
    class VulkanGraphicsShader {
    public:
        VulkanGraphicsShader();
        VulkanGraphicsShader(const VulkanGraphicsShader &) = delete;
        VulkanGraphicsShader(VulkanGraphicsShader &&) = delete;
        ~VulkanGraphicsShader() = default;

        void LoadShaders(const std::filesystem::path &vertexShaderPath, const std::filesystem::path &fragmentShaderPath);

        inline VulkanShaderModule GetShaderModule(const GraphicsShaderType &shaderType) const noexcept;
        
    private:
        std::array<VulkanShaderModule, 2> m_ShaderModules;
    };

#include "VulkanGraphicsShader.inl"
    
}
}
