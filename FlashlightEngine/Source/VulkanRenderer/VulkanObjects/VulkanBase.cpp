/* Copyright (C) 2024 Jean "Pixfri" Letessier (jean.letessier@protonmail.com)
 * This file is part of "FlashLight Engine"
 * For conditions of distribution and use, see copyright notice in FlashLightEngine.hpp
 *
 * VulkanInstance.cpp - Definitions of methods from the VulkanDevice class.
 * This class contains the definitions of methods from the Flashlight::VulkanDevice class.
 */

#include "FlashlightEngine/VulkanRenderer/VulkanObjects/VulkanBase.hpp"
#include "FlashlightEngine/defines.hpp"
#include "FlashlightEngine/pch.hpp"

#include <volk.h>

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
    auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));

    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

/// @ingroup VulkanRenderer
/// @class Flashlight::VulkanBase
/// @brief VulkanRenderer wrapper class for base Vulkan objects.

/// @brief Constructor for the VulkanBase class. Calls the function to initialize
/// every Vulkan object.
///
/// @param window The window of the application using the renderer.
VulkanBase::VulkanBase(const Window &window) : m_Window(window) {
    Init();
}

/// @brief Destructor of the VulkanBase class. Destroys native Vulkan
/// objects created in this class.
VulkanBase::~VulkanBase() {
    Cleanup();
}

/// @brief Creates an instance of this class and returns a std::unqiue_pointer of it.
///
/// @returns A std::unique_pointer to the created object.
std::unique_ptr<VulkanBase> VulkanBase::Create(const Window &window) {
    return std::make_unique<VulkanBase>(window);
}

/// @brief Initializes all of the Vulkan objects this class is a wrapper of.
void VulkanBase::Init() {
    CreateInstance();
    CreateDebugMessenger();
    PickPhysicalDevice();
    CreateLogicalDevice();
}

/// @brief Destroys all the Vulkan objects this class is a wrapper of.
void VulkanBase::Cleanup() {
    DestroyLogicalDevice();
    DestroyDebugMessenger();
    DestroyInstance();
}

/// @brief Initializes the Vulkan instance.
void VulkanBase::CreateInstance() {
    volkInitialize();

#if defined(FL_DEBUG)
    if (!CheckValidationLayerSupport()) {
        throw std::runtime_error("Validation layers requested, but not available.");
    }
#endif

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
#if defined(FL_DEBUG)
        createInfo.enabledLayerCount = static_cast<uint32_t>(m_ValidationLayers.size());
        createInfo.ppEnabledLayerNames = m_ValidationLayers.data();

        PopulateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = reinterpret_cast<VkDebugUtilsMessengerCreateInfoEXT*>(&debugCreateInfo);
#else
        createInfo.enabledLayerCount = 0;

        createInfo.pNext = nullptr;
#endif

#ifdef __APPLE__
    createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif

    if (vkCreateInstance(&createInfo, nullptr, &m_Vulkan.Instance) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create instance.");
    }

    volkLoadInstance(m_Vulkan.Instance);
}

/// @brief Destroys the Vulkan instance.
void VulkanBase::DestroyInstance() const {
    if (m_Vulkan.Instance) {
        vkDestroyInstance(m_Vulkan.Instance, nullptr);
    }
}

/// @brief Creates and setup the debug messenger.
void VulkanBase::CreateDebugMessenger() {
#if defined(FL_DEBUG)
    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    PopulateDebugMessengerCreateInfo(createInfo);


    if (CreateDebugUtilsMessengerEXT(m_Vulkan.Instance, &createInfo, nullptr, &m_Vulkan.DebugMessenger) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create debug messenger.");
    }
#endif
}

/// @brief Destroys the debug messenger.
void VulkanBase::DestroyDebugMessenger() const {
#if defined(FL_DEBUG)
        DestroyDebugUtilsMessengerEXT(m_Vulkan.Instance, m_Vulkan.DebugMessenger, nullptr);
#endif
}


/// @brief Picks a Vulkan physical device.
void VulkanBase::PickPhysicalDevice() {
    uint32 deviceCount = 0;
    vkEnumeratePhysicalDevices(m_Vulkan.Instance, &deviceCount, nullptr);

    if (deviceCount == 0) {
        throw std::runtime_error("Failed to find GPUs with Vulkan support.");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(m_Vulkan.Instance, &deviceCount, devices.data());

    std::multimap<int, VkPhysicalDevice> candidates;

    for (const auto &device : devices) {
        int score = RateDeviceSuitability(device);
        candidates.insert(std::make_pair(score, device));
    }

    if (candidates.rbegin()->first > 0) {
        m_Vulkan.PhysicalDevice = candidates.rbegin()->second;
    } else {
        throw std::runtime_error("Failed to find a suitable GPU");
    }

    auto deviceProperties = GetDeviceProperties(m_Vulkan.PhysicalDevice);

    std::cout << "GPU Informations : " << '\n'
    << '\t' << "GPU Name : " << deviceProperties.deviceName << '\n'
    << '\t' <<"GPU Driver Version : " << deviceProperties.driverVersion << '\n'
    << '\t' << "GPU Vulkan API Version : " << deviceProperties.apiVersion << '\n';
}

/// @brief Creates the Vulkan logical device.
void VulkanBase::CreateLogicalDevice() {
    QueueFamilyIndices indices = FindQueueFamilies(m_Vulkan.PhysicalDevice);

    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = indices.GraphicsFamily;
    queueCreateInfo.queueCount = 1;

    float queuePriority = 1.0f;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    createInfo.queueCreateInfoCount = 1;
    createInfo.pQueueCreateInfos = &queueCreateInfo;

    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount = 0;

#if defined(FL_DEBUG)
        createInfo.enabledLayerCount = static_cast<uint32_t>(m_ValidationLayers.size());
        createInfo.ppEnabledLayerNames = m_ValidationLayers.data();
#else
        createInfo.enabledLayerCount = false;
#endif

    if (vkCreateDevice(m_Vulkan.PhysicalDevice, &createInfo, nullptr, &m_Vulkan.LogicalDevice) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create logical device.");
    }

    vkGetDeviceQueue(m_Vulkan.LogicalDevice, indices.GraphicsFamily, 0, &m_Vulkan.GraphicsQueue);
}

/// @brief Destroys the Vulkan logical device.
void VulkanBase::DestroyLogicalDevice() const {
    vkDestroyDevice(m_Vulkan.LogicalDevice, nullptr);
}

/// @brief Returns the required instance extension for the application to work.
///
/// @returns A std::vector containing the names of the required instance extensions.
std::vector<const char*> VulkanBase::GetRequiredInstanceExtensions() const noexcept {
    uint32_t glfwRequiredExtensionCount = 0;
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

/// @brief Returns the available instance extensions.
///
/// @returns A std::vector of the instance extensions properties.
std::vector<VkExtensionProperties> VulkanBase::GetAvailableInstanceExtensions() const noexcept {
    uint32 extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

    return extensions;
}


/// @brief Checks if the required instance extensions are avaiable.
///
/// @returns A boolean telling if all of the required instance extensions are available.
bool VulkanBase::CheckRequiredInstanceExtensionsSupport() const noexcept {
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
bool VulkanBase::CheckValidationLayerSupport() const noexcept {
    uint32 layerCount;
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
void VulkanBase::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo) noexcept {
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

/// @brief Gets a physical device properties.
///
/// @param physicalDevice The physical device to get the properties of.
///
/// @returns The physical device properties.
VkPhysicalDeviceProperties VulkanBase::GetDeviceProperties(VkPhysicalDevice physicalDevice) const noexcept {
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);

    return deviceProperties;
}

/// @brief Gets a physical device features.
///
/// @param physicalDevice The physical device to get the properties of.
///
/// @returns The physical device features.
VkPhysicalDeviceFeatures VulkanBase::GetDeviceFeatures(VkPhysicalDevice physicalDevice) const noexcept {
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);

    return deviceFeatures;
}

/// @brief Checks if the provided device suits the requirements.
///
/// @param physicalDevice The physical device to check.
///
/// @returns A boolean telling if the provided device suits the requirements.
bool VulkanBase::IsDeviceSuitable(VkPhysicalDevice physicalDevice) const noexcept {
    auto deviceProperties = GetDeviceProperties(physicalDevice);
    auto deviceFeatures = GetDeviceFeatures(physicalDevice);

    QueueFamilyIndices indices = FindQueueFamilies(physicalDevice);

    return indices.IsComplete();
}

/// @brief Rates devices with a score to help select it.
///
/// @param physicalDevice The physical device to rate.
///
/// @returns The score of the provided device.
int VulkanBase::RateDeviceSuitability(VkPhysicalDevice physicalDevice) const noexcept {
    if (!IsDeviceSuitable(physicalDevice)) {
        // Devices that aren't suitable are scored 0.
        return 0;
    }

    int score = 0;

    auto deviceProperties = GetDeviceProperties(physicalDevice);

    // Discrete GPUs have a significant performance advantage.
    if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
        score += 1000;
    }

    // Maximum possible size of textures affects graphics quality.
    score += static_cast<int>(deviceProperties.limits.maxImageDimension2D);

    return score;
}

/// @brief Retrieve queue families from the physical device and returns them.
///
/// @param physicalDevice The physical device to retrieve the queue families from.
///
/// @returns A QueueFamilyIndices struct.
QueueFamilyIndices VulkanBase::FindQueueFamilies(VkPhysicalDevice physicalDevice) const noexcept {
    QueueFamilyIndices indices;

    uint32 queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto &queueFamily : queueFamilies) {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.GraphicsFamily = i;
            indices.GraphicsFamilyHasValue = true;
        }

        if (indices.IsComplete()) {
            break;
        }

        i++;
    }

    return indices;
}

}