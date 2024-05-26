/* Copyright (C) 2024 Jean "Pixfri" Letessier (jean.letessier@protonmail.com)
 * This file is part of "FlashLight Engine"
 * For conditions of distribution and use, see copyright notice in FlashLightEngine.hpp
 *
 * File : VulkanInstance.cpp
 * Description : This file contains the definitions of methods from the VulkanInstance class.
 */

#include "FlashlightEngine/VulkanWrapper/VulkanInstance.hpp"

#include <volk.h>
#include <GLFW/glfw3.h>

namespace Flashlight {

namespace VulkanWrapper {

    // Local functions for debug callback.

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

        std::stringstream message;

        message << "Validation layer ";

        // Severity
        if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) {
            message << "[VERBOSE] ";
        }

        if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
            message << "[WARNING] ";
        }

        if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
            message << "[ERROR] ";
        }

        // Type
        if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT) {
            message << "[GENERAL] ";
        }

        if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) {
            message << "[VALIDATION] ";
        }

        if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT) {
            message << "[PERFORMANCE] ";
        }

        message << ": " << pCallbackData->pMessage << '\n';

        if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT ||
            messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) {
            std::cout << message.str();
            } else {
                std::cerr << message.str();
            }

        return VK_FALSE;
    }
    
    /// @ingroup VulkanRenderer
    /// @class Flashlight::VulkanWrapper::VulkanInstance
    /// @brief Vulkan wrapper class that handles the Vulkan instance.

    /// @brief Initializes the instance with the passed application info and instance create info.
    void VulkanInstance::Create() {

        if (volkInitialize() != VK_SUCCESS) {
            std::cerr << "Failed to initialize Vulkan loader.";
            throw std::runtime_error("");
        }
        
#if defined(FL_DEBUG)
        if (!CheckValidationLayerSupport()) {
            std::cerr << "Validation layers requested, but not available.";
        }
#endif

        VkApplicationInfo appInfo {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Flashlight Engine Application";
        appInfo.applicationVersion = VK_MAKE_API_VERSION(1, 0, 0, 0);
        appInfo.pEngineName = "Flashlight Engine | Vulkan Renderer";
        appInfo.engineVersion = VK_MAKE_API_VERSION(1, 0, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;
        
        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
            
        createInfo.pApplicationInfo = &appInfo;
        
        auto requiredExtensions = GetRequiredInstanceExtensions();

        createInfo.enabledExtensionCount = static_cast<u32>(requiredExtensions.size());
        createInfo.ppEnabledExtensionNames = requiredExtensions.data();
        
#if defined(FL_DEBUG)
        createInfo.enabledLayerCount = static_cast<u32>(m_ValidationLayers.size());
        createInfo.ppEnabledLayerNames = m_ValidationLayers.data();
        
        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        PopulateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = &debugCreateInfo;
#else
        createInfo.enabledLayerCount = 0;
        createInfo.ppEnabledLayerNames = nullptr;
#endif

#if defined(__APPLE__)
        createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif
        
        if (vkCreateInstance(&createInfo, nullptr, &m_Handle) != VK_SUCCESS) {
            std::cerr << "Failed to create instance.";
        }

        volkLoadInstance(m_Handle);
        
        CheckRequiredInstanceExtensionsSupport();
    }

    /// @brief Destroys the Vulkan instance if it is a valid handle.
    void VulkanInstance::Cleanup() const {
        if (m_Handle) {
            vkDestroyInstance(m_Handle, nullptr);
        }
    }

    /// @brief Checks if validations layers are supported.
    ///
    /// @returns A boolean telling if validation layers are supported.
    bool VulkanInstance::CheckValidationLayerSupport() const noexcept {
        u32 layerCount = 0;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (const char *layerName : m_ValidationLayers) {
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
        createInfo = {
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
            .pNext = nullptr,
            .flags = 0,
            .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
            .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
            .pfnUserCallback = DebugCallback,
            .pUserData = nullptr
        };
    }

    /// @brief Returns the required instance extension for the application to work.
    ///
    /// @returns A std::vector containing the names of the required instance extensions.
    std::vector<const char*> VulkanInstance::GetRequiredInstanceExtensions() const noexcept {
        u32 glfwRequiredExtensionCount = 0;
        const char** glfwRequiredExtensions = glfwGetRequiredInstanceExtensions(&glfwRequiredExtensionCount);

        std::vector<const char*> requiredExtensions(glfwRequiredExtensions, glfwRequiredExtensions + glfwRequiredExtensionCount);

#if defined(FL_DEBUG)
        requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif


#ifdef __APPLE__
        requiredExtensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
#endif

        return requiredExtensions;
    }
    
    /// @brief Checks if the required instance extensions are avaiable.
    ///
    /// @returns A boolean telling if all of the required instance extensions are available.
    void VulkanInstance::CheckRequiredInstanceExtensionsSupport() const {
        auto availableInstanceExtensionsVector = GetAvailableInstanceExtensions();

        std::cout << "Available instance extensions : \n";
        std::unordered_set<std::string> availableExtensions;
        for (const auto &extension : availableInstanceExtensionsVector) {
            std::cout << '\t' << extension.extensionName << '\n';
            availableExtensions.insert(extension.extensionName);
        }

        std::cout << std::endl;

        auto requiredInstanceExtensions = GetRequiredInstanceExtensions();
        std::cout << "Required instance extensions : \n";
        for (const auto &required : requiredInstanceExtensions) {
            std::cout << '\t' << required << '\n';
            if (availableExtensions.find(required) == availableExtensions.end()) {
                std::cerr << "Missing required instance extension.";
            }
        }

        std::cout << std::endl;
    }

    /// @brief Returns the available instance extensions.
    ///
    /// @returns A std::vector of the instance extensions properties.
    std::vector<VkExtensionProperties> VulkanInstance::GetAvailableInstanceExtensions() const noexcept {
        u32 extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> extensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

        return extensions;
    }
}

}