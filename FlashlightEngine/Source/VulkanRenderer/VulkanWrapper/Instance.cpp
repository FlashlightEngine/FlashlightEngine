/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Instance.cpp
 * Description : Definitions of methods from the Instance class.
 */
#include <FlashlightEngine/VulkanRenderer/VulkanWrapper/Instance.hpp>

#include <SDL_vulkan.h>

namespace Flashlight::VulkanRenderer::VulkanWrapper {
    namespace {
        VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
            const VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            const VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void* pUserData) {
            std::stringstream message;

            // Severity
            if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
                message << "[INFO] ";
            }

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

            message << '[' << pCallbackData->messageIdNumber << ' ';
            if (pCallbackData->pMessageIdName) {
                message << pCallbackData->pMessageIdName;
            }

            message << "]: " << pCallbackData->pMessage;

            if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT ||
                messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) {
                Log::EngineTrace(message.str());
            } else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
                Log::EngineWarn(message.str());
            } else {
                Log::EngineError(message.str());
            }

            return VK_FALSE;
        }
    }
    
    Instance::Instance(const Window& window, const DebugLevel& debugLevel) {
        volkInitialize();

        Log::EngineTrace("Creating Vulkan instance & debug messenger...");
        vkb::InstanceBuilder builder;

        u32 extensionCount = 0;
        SDL_Vulkan_GetInstanceExtensions(window.GetNativeWindow(), &extensionCount, nullptr);
        std::vector<const char*> requiredExtensions(extensionCount);
        SDL_Vulkan_GetInstanceExtensions(window.GetNativeWindow(), &extensionCount, requiredExtensions.data());

        Log::EngineTrace("Vulkan instance extensions required by the window:");
        for (const auto extension : requiredExtensions) {
            Log::EngineTrace("\t - {0}", extension);
        }

        builder.set_app_name(window.GetTitle().c_str())
               .set_app_version(1, 0, 0)
               .set_engine_name("Flashlight Engine")
               .set_engine_version(1, 0, 0)
               .request_validation_layers(debugLevel > DebugLevel::None)
               .require_api_version(1, 3, 0);

        VkDebugUtilsMessageSeverityFlagsEXT messageSeverity = 0;

        if (debugLevel >= DebugLevel::Errors) {
            messageSeverity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        }

        if (debugLevel >= DebugLevel::Warnings) {
            messageSeverity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
        }

        if (debugLevel >= DebugLevel::Verbose) {
            messageSeverity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
        }

        if (debugLevel >= DebugLevel::Debug) {
            messageSeverity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;
        }

        constexpr VkDebugUtilsMessageTypeFlagsEXT messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

        builder.add_debug_messenger_severity(messageSeverity)
               .add_debug_messenger_type(messageType)
               .set_debug_callback(DebugCallback);

        auto builderResult = builder.build();

        const vkb::Instance vkbInstance = builderResult.value();

        Log::EngineTrace("Vulkan Instance & debug messenger created.");
        m_Instance = vkbInstance.instance;
        m_DebugMessenger = vkbInstance.debug_messenger;
        m_VkbInstance = vkbInstance;

        volkLoadInstanceOnly(m_Instance);        
    }

    Instance::~Instance() {
        if (m_DebugMessenger) {
            Log::EngineTrace("Destroying Vulkan debug messenger.");
            vkb::destroy_debug_utils_messenger(m_Instance, m_DebugMessenger);
        }

        if (m_Instance) {
            Log::EngineTrace("Destroying Vulkan instance.");
            vkDestroyInstance(m_Instance, nullptr);
        }
    }

    Instance::Instance(Instance&& other) noexcept {
        m_Instance = other.m_Instance;
        m_DebugMessenger = other.m_DebugMessenger;
        m_VkbInstance = std::move(other.m_VkbInstance);

        other.m_Instance = VK_NULL_HANDLE;
        other.m_DebugMessenger = VK_NULL_HANDLE;
    }

    Instance& Instance::operator=(Instance&& other) noexcept {
        m_Instance = other.m_Instance;
        m_DebugMessenger = other.m_DebugMessenger;
        m_VkbInstance = std::move(other.m_VkbInstance);

        other.m_Instance = VK_NULL_HANDLE;
        other.m_DebugMessenger = VK_NULL_HANDLE;

        return *this;
    }
}
