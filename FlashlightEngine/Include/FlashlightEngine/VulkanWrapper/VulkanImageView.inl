#pragma once

inline VulkanImageView::VulkanImageView() : m_Handle(VK_NULL_HANDLE) {}

inline VulkanImageView::~VulkanImageView() {
    if (IsValid()) {
        Destroy();
    }
}

inline void VulkanImageView::Create(const VulkanDevice& device, const VkImage& image, const VkFormat &imageFormat,
                                    const ImageAspect &imageAspect) {
    VkImageViewCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image = image;

    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format = imageFormat;

    createInfo.components.r = VK_COMPONENT_SWIZZLE_R;
    createInfo.components.g = VK_COMPONENT_SWIZZLE_G;
    createInfo.components.b = VK_COMPONENT_SWIZZLE_B;
    createInfo.components.a = VK_COMPONENT_SWIZZLE_A;

    createInfo.subresourceRange.aspectMask = static_cast<VkImageAspectFlags>(imageAspect);
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(device.GetHandle(), &createInfo, nullptr, &m_Handle) != VK_SUCCESS) {
        FL_ERROR("Failed to create image view.")
    }

    m_Device = device.GetHandle();
}

inline void VulkanImageView::Destroy() const {
    if (IsValid()) {
        vkDestroyImageView(m_Device, m_Handle, nullptr);
    }
}

inline bool VulkanImageView::IsValid() const noexcept {
    return m_Handle != nullptr;
}

inline VkImageView VulkanImageView::GetHandle() const noexcept {
    if (!IsValid()) {
        return nullptr;
    }

    return m_Handle;
}