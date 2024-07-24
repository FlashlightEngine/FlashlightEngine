/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Framebuffer.inl
 * Description : Definitions of inline methods from the Framebuffer class.
 */
#pragma once

inline Framebuffer::~Framebuffer() {
    if (m_Framebuffer) {
        Log::EngineTrace("Destroying Vulkan framebuffer.");
        vkDestroyFramebuffer(m_Device.GetNativeDevice(), m_Framebuffer, nullptr);
    }
}

inline Framebuffer::Framebuffer(Framebuffer && rhs) noexcept : m_Device(rhs.m_Device) {
    m_Framebuffer = rhs.m_Framebuffer;
    rhs.m_Framebuffer = VK_NULL_HANDLE;
}

inline Framebuffer& Framebuffer::operator=(Framebuffer&& rhs) noexcept {
    m_Framebuffer = rhs.m_Framebuffer;
    rhs.m_Framebuffer = VK_NULL_HANDLE;
    return *this;
}


inline VkFramebuffer Framebuffer::GetNativeFramebuffer() const {
    return m_Framebuffer;
}