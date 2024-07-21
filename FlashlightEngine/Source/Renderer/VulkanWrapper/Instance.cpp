/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Instance.cpp
 * Description : Definitions of methods from the Instance class.
 */
#include "FlashlightEngine/Renderer/VulkanWrapper/Instance.hpp"

#include "FlashlightEngine/Core/Logger.hpp"

#include "FlashlightEngine/pch.hpp"

#include <GLFW/glfw3.h>

#include <volk.h>

namespace Flashlight::VulkanWrapper {
    void Instance::Create() {
        volkInitialize();
        
        VkApplicationInfo applicationInfo{};
        applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        applicationInfo.pApplicationName = "FlashlightEngine Application";
        applicationInfo.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
        applicationInfo.pEngineName = "FlashlightEngine";
        applicationInfo.engineVersion = VK_MAKE_API_VERSION(0, 0, 0, 1);
        applicationInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &applicationInfo;

        const auto requiredExtensions = GetRequiredInstanceExtensions();

        createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
        createInfo.ppEnabledExtensionNames = requiredExtensions.data();

        createInfo.enabledLayerCount = 0;
        createInfo.ppEnabledLayerNames = nullptr;

        Log::AppTrace("Creating Vulkan instance.");
        if (vkCreateInstance(&createInfo, nullptr, &m_Instance) != VK_SUCCESS) {
            Log::EngineFatal({0x01, 0x00}, "Failed to create Vulkan instance.");
        }
        Log::AppTrace("Vulkan instance created.");

        volkLoadInstanceOnly(m_Instance);

        CheckRequiredInstanceExtensionsSupport();
    }

    std::vector<const char*> Instance::GetRequiredInstanceExtensions() {
        uint32_t glfwRequiredExtensionCount = 0;
        const char** glfwRequiredExtensions = glfwGetRequiredInstanceExtensions(&glfwRequiredExtensionCount);

        std::vector requiredExtensions(glfwRequiredExtensions, glfwRequiredExtensions + glfwRequiredExtensionCount);

        return requiredExtensions;
    }

    void Instance::CheckRequiredInstanceExtensionsSupport() {
        const auto availableInstanceExtensionsVector = GetAvailableInstanceExtensions();

        Log::EngineTrace("Available instance extensions :");

        std::unordered_set<std::string> availableExtensions;

        for (const auto& extension : availableInstanceExtensionsVector) {
            Log::EngineTrace("\t - {0}", extension.extensionName);
            availableExtensions.insert(extension.extensionName);
        }

        std::cout << '\n';

        const auto requiredInstanceExtensions = GetRequiredInstanceExtensions();

        Log::EngineTrace("Required instance extensions :");

        for (const auto& required : requiredInstanceExtensions) {
            Log::EngineTrace("\t - {0}", required);

            if (!availableExtensions.contains(required)) {
                Log::EngineError("Instance extension {0} is required but not available.", required);
            }
        }

        std::cout << '\n';
    }

    std::vector<VkExtensionProperties> Instance::GetAvailableInstanceExtensions() {
        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> extensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

        return extensions;
    }

}
