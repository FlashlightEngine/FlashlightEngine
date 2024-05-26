#pragma once

#include "VulkanDevice.hpp"

#include <volk.h>

#include <filesystem>

namespace Flashlight {

namespace VulkanWrapper {
    
    class VulkanShaderModule {
    public:
        inline VulkanShaderModule();
        VulkanShaderModule(const VulkanShaderModule &) = delete;
        VulkanShaderModule(VulkanShaderModule &&) = delete;
        inline ~VulkanShaderModule();

        inline bool IsValid() const noexcept;
        inline VkShaderModule GetHandle() const noexcept;

        inline void Create(const VulkanDevice& device, const std::filesystem::path &shaderPath);
        inline void Destroy() const;
        
    private:
        VkDevice m_Device;
        VkShaderModule m_Handle;
    };

#include "VulkanShaderModule.inl"
    
}

}
