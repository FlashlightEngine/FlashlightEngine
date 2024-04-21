#pragma once

inline VulkanWindowSurface::VulkanWindowSurface() : m_Handle(VK_NULL_HANDLE) {}

inline VulkanWindowSurface::~VulkanWindowSurface() {
    if (IsValid()) {
        Destroy();
    }
}

inline void VulkanWindowSurface::Create(VulkanInstance &instance, Window &window) {
    m_Instance = &instance;
    CreateHandle(m_Instance, window);
}

inline void VulkanWindowSurface::Destroy() {
    if (IsValid()) {
        DestroyHandle(m_Instance, m_Handle);
    }
}

inline void VulkanWindowSurface::CreateHandle(VulkanInstance &instance, Window &window) {
    if (glfwCreateWindowSurface(instance.GetHandle(), window.GetNativeWindow(), nullptr, &m_Handle) != VK_SUCCESS) {
        Log::EngineError("Failed to create window surface.");
    }
}

inline void VulkanWindowSurface::DestroyHandle(VulkanInstance &instance, VkSurfaceKHR &surface) {
    instance.vkDestroySurfaceKHR(instance.GetHandle(), surface, nullptr);
}


