/* Copyright (C) 2024 Jean "Pixfri" Letessier (jean.letessier@protonmail.com)
 * This file is part of "FlashLight Engine"
 * For conditions of distribution and use, see copyright notice in FlashLightEngine.hpp
 *
 * VulkanInstance.hpp - The definition of the VulkanInstance class.
 * This header file contains the definition of the VulkanInstance class, which is a wrapper for the VkInstance.
 */

#pragma once

#include "pch.hpp"
#include "Core/Window.hpp"

namespace Flashlight {

    class VulkanInstance {
    public:
        explicit VulkanInstance(const Window &window);
        ~VulkanInstance();

        // We can't copy/move vulkan objects.
        VulkanInstance(const VulkanInstance &) = delete;
        VulkanInstance &operator=(const VulkanInstance &) = delete;
        VulkanInstance(VulkanInstance &&) = delete;
        VulkanInstance &operator=(VulkanInstance &&) = delete;

        static std::unique_ptr<VulkanInstance> Create(const Window &window);

    private:
        VkInstance m_Instance{};

        Window m_Window;

        void Init();
        void Cleanup();

        // Function to create Vulkan objects and to destroy them.
        void CreateInstance();
        void DestroyInstance();

        // Utility functions.
        std::vector<const char*> GetRequiredInstanceExtensions() const noexcept;
        std::vector<VkExtensionProperties> GetAvailableInstanceExtensions() const noexcept;
        bool CheckRequiredInstanceExtensionsSupport() const noexcept;

        const std::vector<const char*> validationLayers = {
            "VK_LAYER_KHRONOS_validation"
        };

#ifdef NDEBUG
        const bool enableValidationLayers = false;
#else
        const bool enableValidationLayers = true;
#endif
    };

}