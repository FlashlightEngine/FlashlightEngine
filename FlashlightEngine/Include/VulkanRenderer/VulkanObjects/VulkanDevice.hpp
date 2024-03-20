#pragma once

#include "pch.hpp"
#include "Core/Window.hpp"

namespace Flashlight {

class VulkanDevice {
public:
    explicit VulkanDevice(const Window& window);
    ~VulkanDevice();

    static std::unique_ptr<VulkanDevice> Create(const Window& window);

private:
    Window m_Window;

    VkInstance m_Instance{};

    void Init();
    void Cleanup();

    // Function to create Vulkan objects and to destroy them.
    void CreateInstance();
    void DestroyInstance();

    // Utility functions.
    std::vector<const char*> GetRequiredInstanceExtensions() const noexcept;
    std::vector<VkExtensionProperties> GetAvailableInstanceExtensions() const noexcept;
    bool CheckRequiredInstanceExtensionsSupport() const noexcept;
};

}