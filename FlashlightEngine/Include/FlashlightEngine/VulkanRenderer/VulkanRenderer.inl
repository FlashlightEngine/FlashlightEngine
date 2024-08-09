/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : VulkanRenderer.inl
 * Description : Definitions of inline methods from the VulkanRenderer class.
 */
#pragma once

inline VulkanWrapper::Instance& VulkanRenderer::GetInstance() const {
    return *m_Instance;
}

inline VulkanWrapper::Device& VulkanRenderer::GetDevice() const {
    return *m_Device;
}

inline VmaAllocator VulkanRenderer::GetAllocator() const {
    return m_Allocator;
}

inline VkFormat VulkanRenderer::GetDrawImageFormat() const {
    return DrawImage.ImageFormat;
}

inline VkFormat VulkanRenderer::GetDepthImageFormat() const {
        return DepthImage.ImageFormat;
}