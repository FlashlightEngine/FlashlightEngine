#pragma once

#include "FlashlightEngine/VulkanWrapper/VulkanDevice.hpp"

#include <volk.h>

namespace Flashlight {

namespace VulkanWrapper {

    struct VulkanRenderPassAttachments {
        VkAttachmentDescription ColorAttachmentDescription;
        VkAttachmentReference ColorAttachmentReference;
        // VkAttachmentDescription DepthAttachmentDescription;
        // VkAttachmentReference DepthAttachmentReference;

        inline static void UseDefaultAttachments(VulkanRenderPassAttachments &attachments, const VkFormat &format);
    };

    class VulkanRenderPass {
    public:
        inline VulkanRenderPass();
        VulkanRenderPass(const VulkanRenderPass &) = delete;
        VulkanRenderPass(VulkanRenderPass &&) = delete;
        inline ~VulkanRenderPass();

        inline bool IsValid() const noexcept;
        inline VkRenderPass GetHandle() const noexcept;

        inline void Create(const VulkanDevice &device, const VulkanRenderPassAttachments &colorAttachments);
        inline void Destroy() const;
        
    private:
        VkDevice m_Device;
        VkRenderPass m_Handle;
    };

#include "VulkanRenderPass.inl"
    
}

}
