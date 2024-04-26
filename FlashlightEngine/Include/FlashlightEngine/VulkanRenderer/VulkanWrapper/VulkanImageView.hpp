#pragma once

#include <volk.h>

#include "VulkanDevice.hpp"

namespace Flashlight {

namespace VulkanWrapper {

    enum class ImageAspect : VkImageAspectFlags {
        Color = VK_IMAGE_ASPECT_COLOR_BIT,
        Stencil = VK_IMAGE_ASPECT_STENCIL_BIT,
        Depth = VK_IMAGE_ASPECT_DEPTH_BIT
    };

    class VulkanImageView {
    public:
        inline VulkanImageView();
        VulkanImageView(const VulkanImageView &) = delete;
        VulkanImageView(VulkanImageView &&) = default;
        inline ~VulkanImageView();

        inline bool IsValid() const noexcept;
        inline VkImageView GetHandle() const noexcept;
        
        inline void Create(const VulkanDevice &device, const VkImage &image, const VkFormat &imageFormat, const ImageAspect &imageAspect);
        inline void Destroy() const;
        
    private:
        VkDevice m_Device;
        VkImageView m_Handle;
    };

#include "VulkanImageView.inl"
    
}

}
