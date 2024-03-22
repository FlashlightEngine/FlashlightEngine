#pragma once

#include "pch.hpp"
#include "Core/Window.hpp"

namespace Flashlight {

    class VulkanInstance {
    public:
        VulkanInstance();
        ~VulkanInstance();

        static std::unique_ptr<VulkanInstance> Create();

        VkInstance GetNativeInstance() const noexcept;

    private:
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