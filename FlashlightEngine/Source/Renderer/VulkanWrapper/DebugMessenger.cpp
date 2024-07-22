/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : DebugMessenger.cpp
 * Description : Definitions of methods from the DebugMessenger class.
 */
#include "FlashlightEngine/Renderer/VulkanWrapper/DebugMessenger.hpp"

namespace Flashlight::VulkanWrapper {
    void DebugMessenger::Create(const DebugLevel& debugLevel) {
        if (debugLevel == DebugLevel::None)
            return;

        VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo{};
        Instance::PopulateDebugMessengerCreateInfo(debugMessengerCreateInfo, debugLevel);

        Log::EngineTrace("Creating Vulkan debug messenger.");
        if (vkCreateDebugUtilsMessengerEXT(m_Instance, &debugMessengerCreateInfo, nullptr, &m_DebugMessenger) !=
            VK_SUCCESS) {
            Log::EngineError("Failed to create Vulkan debug messenger.");
        } else {
            Log::EngineTrace("Vulkan debug messenger created.");
        }
    }

}
