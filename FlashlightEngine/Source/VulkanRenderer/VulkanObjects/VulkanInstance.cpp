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

// Local functions for debug callback and debug messenger creation.

/// @brief Debug callback for the debug messenger.
///
/// @param messageSeverity The severity of the message.
/// @param messageType The type of the message.
/// @param pCallbackData A pointer to the data passed to the callback function.
/// @param pUserData Custom data you can pass to the callback.
///
/// @returns This function always returns VK_FALSE. If it returns VK_TRUE, it aborts the Vulkan call is aborted. This should only be used to test validation layers themselves.
static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
    void *pUserData) {

    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}

/// @brief Loads the function to create a debug messenger and calls it.
///
/// @param instance The Vulkan instance.
/// @param pCreateInfo A pointer to the VkDebugUtilsMessengerCreateInfoEXT struct.
/// @param pAllocator Optional. A pointer to the custom allocator.
/// @param pDebugMessenger A pointer to the debug messenger to create.
///
/// @returns Returns the result of the loaded function. VK_SUCCESS if the function succeeded, VK_ERROR_EXTENSION_NOT_PRESENT
/// if the function couldn't be loaded, or any other error the loaded extension might produce.
VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                      const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger) {

    auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));

    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

/// @brief Loads the function to destroy a debug messenger and calls it.
///
/// @param instance The Vulkan instance.
/// @param debugMessenger The debug messenger to destroy.
/// @param pAllocator Optional. A pointer to the custom allocator.
void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks *pAllocator) {
    auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));

    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

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
    CreateDebugMessenger();
}

/// @brief Destroys all of the Vulkan objects this class is a wrapper of.
void VulkanInstance::Cleanup() {
    DestroyDebugMessenger();
    DestroyInstance();
}

/// @brief Initializes the Vulkan instance.
void VulkanInstance::CreateInstance() {
    if (m_EnableValidationLayers && !CheckValidationLayerSupport()) {
        throw std::runtime_error("Validation layers requested, but not available.");
    }

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Flashlight Engine Application";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Flashlight Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    auto requiredExtensions = GetRequiredInstanceExtensions();
    auto availableExtensions = GetAvailableInstanceExtensions();

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

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if (m_EnableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(m_validationLayers.size());
        createInfo.ppEnabledLayerNames = m_validationLayers.data();

        PopulateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = reinterpret_cast<VkDebugUtilsMessengerCreateInfoEXT*>(&debugCreateInfo);
    } else {
        createInfo.enabledLayerCount = 0;

        createInfo.pNext = nullptr;
    }

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

/// @brief Creates and setup the debug messenger.
void VulkanInstance::CreateDebugMessenger() {
    if (!m_EnableValidationLayers) return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    PopulateDebugMessengerCreateInfo(createInfo);


    if (CreateDebugUtilsMessengerEXT(m_Instance, &createInfo, nullptr, &m_DebugMessenger) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create debug messenger.");
    }
}

/// @brief Destroys the debug messenger.
void VulkanInstance::DestroyDebugMessenger() {
    if (m_EnableValidationLayers) {
        DestroyDebugUtilsMessengerEXT(m_Instance, m_DebugMessenger, nullptr);
    }
}

/// @brief Returns the required instance extension for the application to work.
///
/// @returns A std::vector containing the names of the required instance extensions.
std::vector<const char*> VulkanInstance::GetRequiredInstanceExtensions() const noexcept {
    uint32_t glfwRequiredExtensionCount = 0;
    const char** glfwRequiredExtensions = glfwGetRequiredInstanceExtensions(&glfwRequiredExtensionCount);

    std::vector<const char*> requiredExtensions(glfwRequiredExtensions, glfwRequiredExtensions + glfwRequiredExtensionCount);

    if (m_EnableValidationLayers) {
        requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
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

/// @brief Checks if validations layers are supported.
///
/// @returns A boolean telling if validation layers are supported.
bool VulkanInstance::CheckValidationLayerSupport() const noexcept {
    uint32 layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char *layerName : m_validationLayers) {
        bool layerFound = false;

        for (const auto &layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            return false;
        }
    }

    return true;
}

/// @brief Fills the VkDebugUtilsMessengerCreateInfoEXT passed to the function.
///
/// @param createInfo The `VkDebugUtilsMessengerCreateInfoEXT` to fill.
void VulkanInstance::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo) noexcept {
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = DebugCallback;
    createInfo.pUserData = nullptr;
}

}