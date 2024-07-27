/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : DebugMessenger.cpp
 * Description : Definitions of methods from the DebugMessenger class.
 */
#include "FlashlightEngine/Renderer/VulkanWrapper/DebugMessenger.hpp"

namespace Flashlight::VulkanWrapper {
    DebugMessenger::DebugMessenger(const std::shared_ptr<Instance>& instance, const DebugLevel& debugLevel) : m_Instance(instance) {
        if (debugLevel == DebugLevel::None)
            return;

        VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo{};
        Instance::PopulateDebugMessengerCreateInfo(debugMessengerCreateInfo, debugLevel);

        Log::EngineTrace("Creating Vulkan debug messenger...");
        if (vkCreateDebugUtilsMessengerEXT(m_Instance->GetNativeInstance(), &debugMessengerCreateInfo, nullptr,
                                           &m_DebugMessenger) != VK_SUCCESS) {
            Log::EngineFatal({0x01, 0x02}, "Failed to create Vulkan debug messenger.");
        } else {
            Log::EngineTrace("Vulkan debug messenger created.");
        }
    }
}
