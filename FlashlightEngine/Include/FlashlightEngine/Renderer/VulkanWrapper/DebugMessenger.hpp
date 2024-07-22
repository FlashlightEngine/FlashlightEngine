#pragma once

#include "FlashlightEngine/Renderer/VulkanWrapper/Instance.hpp"

#include "FlashlightEngine/Core/Logger.hpp"

#include <volk.h>

namespace Flashlight::VulkanWrapper {
    class DebugMessenger {
        VkDebugUtilsMessengerEXT m_DebugMessenger = VK_NULL_HANDLE;
        VkInstance m_Instance = VK_NULL_HANDLE;
        
    public:
        inline DebugMessenger(const Instance& instance, const DebugLevel& debugLevel);
        inline ~DebugMessenger();

        DebugMessenger(const DebugMessenger&) = delete;
        DebugMessenger(DebugMessenger&&) = delete;

        DebugMessenger& operator=(const DebugMessenger&) = delete;
        DebugMessenger& operator=(DebugMessenger&&) = delete;

        [[nodiscard]] inline VkDebugUtilsMessengerEXT GetNativeDebugMessenger() const;

    private:
        void Create(VkInstance instance, const DebugLevel &debugLevel);
        inline void Destroy() const;
    };

#include "DebugMessenger.inl"
}
