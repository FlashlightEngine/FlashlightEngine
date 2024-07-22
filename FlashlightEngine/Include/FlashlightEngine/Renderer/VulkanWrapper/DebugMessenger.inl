#pragma once

inline DebugMessenger::DebugMessenger(const Instance& instance, const DebugLevel& debugLevel) {
    Create(instance.GetNativeInstance(), debugLevel);
    m_Instance = instance.GetNativeInstance();
}

inline DebugMessenger::~DebugMessenger() {
    Destroy();
}

inline VkDebugUtilsMessengerEXT DebugMessenger::GetNativeDebugMessenger() const {
    return m_DebugMessenger;
}

inline void DebugMessenger::Destroy() const {
    if (m_DebugMessenger) {
        Log::EngineTrace("Destroying Vulkan debug messenger.");
        vkDestroyDebugUtilsMessengerEXT(m_Instance, m_DebugMessenger, nullptr);
    }
}
