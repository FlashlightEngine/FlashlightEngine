#pragma once

#include "FlashlightEngine/Core/Log.hpp"

inline VulkanWindowSurface::VulkanWindowSurface() : m_Instance(VK_NULL_HANDLE), m_Handle(VK_NULL_HANDLE) {}

inline VulkanWindowSurface::~VulkanWindowSurface() {
    if (IsValid()) {
        Destroy();
    }
}

inline void VulkanWindowSurface::Create(const VulkanInstance &instance, Window &window) {
    if (glfwCreateWindowSurface(instance.GetInstance(), window.GetNativeWindow(), nullptr, &m_Handle)
        != VK_SUCCESS) {
        FL_THROW("Failed to create window surface.")
    }

    m_Instance = instance.GetInstance();
}

inline void VulkanWindowSurface::Destroy() const {
    if (IsValid()) {
        vkDestroySurfaceKHR(m_Instance, m_Handle, nullptr);
    }
}

inline bool VulkanWindowSurface::IsValid() const noexcept {
    return m_Handle != nullptr;
}

inline VkSurfaceKHR VulkanWindowSurface::GetSurface() const noexcept {
    if (!IsValid()) {
        return nullptr;
    }

    return m_Handle;
}
