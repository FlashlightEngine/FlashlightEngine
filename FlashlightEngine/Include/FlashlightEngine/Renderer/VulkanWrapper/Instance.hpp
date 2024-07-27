/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Instance.hpp
 * Description : Declaration of the Instance class. An instance within Vulkan is an object used to store per-application
 *               states. It therefor stores per-application information and is used to create other per-application
 *               objects.
 */
#pragma once

#include "FlashlightEngine/Core/Logger.hpp"

#include <volk.h>

namespace Flashlight {
    enum class DebugLevel {
        None = 0,
        Errors = 1,
        Warnings = 2,
        Verbose = 3,
        Debug = 4
    };

    namespace VulkanWrapper {
        class Instance {
            VkInstance m_Instance = VK_NULL_HANDLE;

            std::vector<const char*> m_ValidationLayers = {
                "VK_LAYER_KHRONOS_validation"
            };

        public:
            explicit Instance(const DebugLevel& debugLevel);
            inline ~Instance();

            Instance(const Instance&) = delete;
            Instance(Instance&&) = delete;

            Instance& operator=(const Instance&) = delete;
            Instance& operator=(Instance&&) = delete;

            [[nodiscard]] inline VkInstance GetNativeInstance() const;
            [[nodiscard]] inline std::vector<const char*> GetValidationLayers() const;

            static void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo,
                                                         const DebugLevel& debugLevel);

        private:

            [[nodiscard]] static std::vector<const char*> GetRequiredInstanceExtensions(
                const DebugLevel& debugLevel);
            static void CheckRequiredInstanceExtensionsSupport(const DebugLevel& debugLevel);
            [[nodiscard]] static std::vector<VkExtensionProperties> GetAvailableInstanceExtensions();

            [[nodiscard]] bool CheckValidationLayerSupport() const;
        };

#include "Instance.inl"
    }
}
