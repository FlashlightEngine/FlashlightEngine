#pragma once

#include "VulkanDevice.hpp"

#include <volk.h>

#include <filesystem>

namespace Flashlight {

namespace VulkanWrapper {

    enum class ShaderType : VkShaderStageFlags {
        Vertex = VK_SHADER_STAGE_VERTEX_BIT,
        Fragment = VK_SHADER_STAGE_FRAGMENT_BIT
    };

    class VulkanGraphicsShader {
    public:
        inline VulkanGraphicsShader();
        VulkanGraphicsShader(const VulkanGraphicsShader &) = delete;
        VulkanGraphicsShader(VulkanGraphicsShader &&) = delete;
        inline ~VulkanGraphicsShader();

        inline bool IsValid() const noexcept;
        inline VkShaderModule GetHandle() const noexcept;

        inline void Create(const VulkanDevice& device, const std::filesystem::path &shaderPath);
        inline void Destroy() const;
        
    private:
        VkDevice m_Device;
        VkShaderModule m_Handle;
    };

#include "FlashlightEngine/VulkanRenderer/VulkanWrapper/VulkanShaderModule.inl"
    
}

}
