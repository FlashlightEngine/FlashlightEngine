#include "FlashlightEngine/Renderer/VulkanWrapper/DebugMessenger.hpp"

namespace Flashlight::VulkanWrapper {
    void DebugMessenger::Create(const VkInstance instance, const DebugLevel& debugLevel) {
        if (debugLevel == DebugLevel::None)
            return;

        VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo{};
        Instance::PopulateDebugMessengerCreateInfo(debugMessengerCreateInfo, debugLevel);

        Log::EngineTrace("Creating Vulkan debug messenger.");
        if (vkCreateDebugUtilsMessengerEXT(instance, &debugMessengerCreateInfo, nullptr, &m_DebugMessenger) !=
            VK_SUCCESS) {
            Log::EngineError("Failed to create Vulkan debug messenger.");
        } else {
            Log::EngineTrace("Vulkan debug messenger created.");
        }
    }

}
