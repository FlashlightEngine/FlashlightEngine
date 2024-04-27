#pragma once

/// @ingroup VulkanRenderer
/// @class Flashlight::VulkanWrapper::VulkanDebugMessenger
/// @brief Wrapper class for the debug messenger in Vulkan.

/// @brief Constructor for this class. Initializes members to null.
inline VulkanDebugMessenger::VulkanDebugMessenger() : m_Handle(VK_NULL_HANDLE), m_Instance(VK_NULL_HANDLE) {}

/// @brief Destroys the debug messenger if it is a valid handle;
inline VulkanDebugMessenger::~VulkanDebugMessenger() {
    if (IsValid()) {
        Destroy();
    }
}

/// @brief Fills the VkDebugUtilsMessengerCreateInfoEXT struct passed and creates the debug messenger with it.
///
/// @param instance The instance the debug messenger depends on.
/// @param createInfo The VkDebugUtilsMessengerCreateInfoEXT to be filled.
inline void VulkanDebugMessenger::Create(VulkanInstance& instance) {

    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    
    instance.PopulateDebugMessengerCreateInfo(createInfo);

    if (vkCreateDebugUtilsMessengerEXT(instance.GetHandle(), &createInfo, nullptr, &m_Handle) != VK_SUCCESS) {
        std::cerr << "(Vulkan Renderer) Failed to create debug messenger.";
        throw std::runtime_error("");
    }

    m_Instance = instance.GetHandle();
}

/// @brief Destroys the VkDebugUtilsMessengerEXT handle if it is valid.
inline void VulkanDebugMessenger::Destroy() const {
    if (IsValid()) {
        vkDestroyDebugUtilsMessengerEXT(m_Instance, m_Handle, nullptr);
    }
}

/// @brief Returns true if the handle is not null.
///
/// @returns true if the handle is valid.
inline bool VulkanDebugMessenger::IsValid() const noexcept {
    return m_Handle != nullptr;
}

/// @brief returns the handle to the VkDebugUtilsMessengerEXT struct.
///
/// @returns The handle if it is valid, nullptr if it is not.
inline VkDebugUtilsMessengerEXT VulkanDebugMessenger::GetHandle() const noexcept {
    if (!IsValid()) {
        return nullptr;
    }

    return m_Handle;
}

