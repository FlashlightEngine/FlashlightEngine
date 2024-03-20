#include "VulkanRenderer/VulkanObjects/VulkanDevice.hpp"

#include "pch.hpp"

namespace Flashlight {
VulkanDevice::VulkanDevice(const Window& window) : m_Window(window) {
    Init();
}

VulkanDevice::~VulkanDevice() {
    Cleanup();
}

std::unique_ptr<VulkanDevice> VulkanDevice::Create(const Window& window) {
    return std::make_unique<VulkanDevice>(window);
}

void VulkanDevice::Init() {
    CreateInstance();
}

void VulkanDevice::Cleanup() {
    DestroyInstance();
}

void VulkanDevice::CreateInstance() {
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

void VulkanDevice::DestroyInstance() {
    if (m_Instance) {
        vkDestroyInstance(m_Instance, nullptr);
    }
}

std::vector<const char*> VulkanDevice::GetRequiredInstanceExtensions() const noexcept {
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

std::vector<VkExtensionProperties> VulkanDevice::GetAvailableInstanceExtensions() const noexcept {
    uint32 extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

    return extensions;
}

bool VulkanDevice::CheckRequiredInstanceExtensionsSupport() const noexcept {
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