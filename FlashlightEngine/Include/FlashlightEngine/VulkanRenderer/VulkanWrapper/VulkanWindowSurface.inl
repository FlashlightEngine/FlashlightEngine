#pragma once

inline VulkanWindowSurface::VulkanWindowSurface() : m_Instance(VK_NULL_HANDLE), m_Handle(VK_NULL_HANDLE) {}

inline VulkanWindowSurface::~VulkanWindowSurface() {
    if (IsValid()) {
        Destroy();
    }
}

inline void VulkanWindowSurface::Create(const VulkanInstance &instance, Window &window) {
    if (glfwCreateWindowSurface(instance.GetHandle(), window.GetNativeWindow(), nullptr, &m_Handle)
        != VK_SUCCESS) {
        Log::EngineError("Failed to create window surface.");
    }

    m_Instance = instance.GetHandle();
}

inline void VulkanWindowSurface::Destroy() const {
    if (IsValid()) {
        vkDestroySurfaceKHR(m_Instance, m_Handle, nullptr);
    }
}

inline bool VulkanWindowSurface::IsValid() const noexcept {
    return m_Handle != nullptr;
}

inline VkSurfaceKHR VulkanWindowSurface::GetHandle() const noexcept {
    if (!IsValid()) {
        return nullptr;
    }

    return m_Handle;
}