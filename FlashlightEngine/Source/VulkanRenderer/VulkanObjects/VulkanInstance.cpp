/* Copyright (C) 2024 Jean "Pixfri" Letessier (jean.letessier@protonmail.com)
 * This file is part of "FlashLight Engine"
 * For conditions of distribution and use, see copyright notice in FlashLightEngine.hpp
 *
 * VulkanInstance.cpp - Definitions of methods from the VulkanInstance class.
 * This class contains the definitions of methods from the Flashlight::VulkanInstance class.
 */

#include "VulkanRenderer/VulkanObjects/VulkanInstance.hpp"

#include "pch.hpp"

namespace Flashlight {

/// @ingroup VulkanRenderer
/// @class Flashlight::VulkanInstance
/// @brief VulkanRenderer class that represents the Vulkan instance.

/// @brief Constructor for the VulkanInstance class. Initializes the Vulkan
/// instance and the debug messenger.
///
/// @param window The window of the application using the instance
VulkanInstance::VulkanInstance(const Window &window) : m_Window(window) {
    Init();
}

/// @brief Destructor of the VulkanInstance class. Destroys native Vulkan
/// objects created in this class.
VulkanInstance::~VulkanInstance() {
    Cleanup();
}

/// @brief Creates an instance and returns a std::unqiue_pointer of it.
///
/// @returns A std::unique_pointer to the window.
std::unique_ptr<VulkanInstance> VulkanInstance::Create(const Window &window) {
    return std::make_unique<VulkanInstance>(window);
}

/// @brief Initializes all of the Vulkan objects this class is a wrapper of.
void VulkanInstance::Init() {
    CreateInstance();
}

/// @brief Destroys all of the Vulkan objects this class is a wrapper of.
void VulkanInstance::Cleanup() {
    DestroyInstance();
}

/// @brief Initializes the Vulkan instance.
void VulkanInstance::CreateInstance() {
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    appInfo.pApplicationName = "Pixfri Engine Application";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Pixfri Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    std::vector<const char*> requiredExtensions = GetRequiredInstanceExtensions();
    std::vector<VkExtensionProperties> availableExtensions = GetAvailableInstanceExtensions();

    std::cout << "Required instance extensions :\n";
    for (const auto &extension : requiredExtensions) {
        std::cout << '\t' << extension << '\n';
    }

    std::cout << "Available instance extensions :\n";
    for (const auto &extension : availableExtensions) {
        std::cout << '\t' << extension.extensionName << '\n';
    }

    if (!CheckRequiredInstanceExtensionsSupport()) {
        throw std::runtime_error("A required instance extension isn't supported.");
    }

    createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
    createInfo.ppEnabledExtensionNames = requiredExtensions.data();

#ifdef __APPLE__
    createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif

    if (vkCreateInstance(&createInfo, nullptr, &m_Instance) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create instance.");
    }
}

/// @brief Destroys the Vulkan instance.
void VulkanInstance::DestroyInstance() {
    if (m_Instance) {
        vkDestroyInstance(m_Instance, nullptr);
    }
}

/// @brief Returns the required instance extension for the application to work.
///
/// @returns A std::vector containing the names of the required instance extensions.
std::vector<const char*> VulkanInstance::GetRequiredInstanceExtensions() const noexcept {
    uint32_t glfwRequiredExtensionCount = 0;
    const char** glfwRequiredExtensions = glfwGetRequiredInstanceExtensions(&glfwRequiredExtensionCount);

    std::vector<const char*> requiredExtensions;

    for (uint32 i = 0; i < glfwRequiredExtensionCount; i++) {
        requiredExtensions.emplace_back(glfwRequiredExtensions[i]);
    }

#ifdef __APPLE__
        requiredExtensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
#endif

    return requiredExtensions;
}

/// @brief Returns the available instance extensions.
///
/// @returns A std::vector of the instance extensions properties.
std::vector<VkExtensionProperties> VulkanInstance::GetAvailableInstanceExtensions() const noexcept {
    uint32 extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

    return extensions;
}


/// @brief Checks if the required instance extensions are avaiable.
///
/// @returns A boolean telling if all of the required instance extensions are available.
bool VulkanInstance::CheckRequiredInstanceExtensionsSupport() const noexcept {
    auto requiredInstanceExtensions = GetRequiredInstanceExtensions();
    std::set<std::string> requiredExtensions(requiredInstanceExtensions.begin(), requiredInstanceExtensions.end());

    auto availableInstanceExtensions = GetAvailableInstanceExtensions();

    for (const auto &extension : availableInstanceExtensions) {
        requiredExtensions.erase(extension.extensionName);

        if (requiredExtensions.empty()) {
            break;
        }
    }

    return requiredExtensions.empty();
}

}