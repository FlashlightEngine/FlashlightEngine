/* Copyright (C) 2024 Jean "Pixfri" Letessier (jean.letessier@protonmail.com)
 * This file is part of "FlashLight Engine"
 * For conditions of distribution and use, see copyright notice in FlashLightEngine.hpp
 *
 * File : VulkanBase.cpp
 * Description : This file contains the definitions of methods from the Flashlight::VulkanDevice class.
 */

#include "FlashlightEngine/VulkanRenderer/VulkanWrapper/VulkanBase.hpp"

#include <volk.h>
#include <GLFW/glfw3.h>

#include "FlashlightEngine/pch.hpp"

namespace Flashlight {

namespace VulkanWrapper
{
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

        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

        return VK_FALSE;
    }

    /// @ingroup VulkanRenderer
    /// @class Flashlight::VulkanBase
    /// @brief VulkanRenderer wrapper class for base Vulkan objects.

    /// @brief Initializes all of the Vulkan objects this class is a wrapper of.
    void VulkanBase::Init() {
        CreateInstance();
        CreateDebugMessenger();
        CreateSurface();
        PickPhysicalDevice();
        CreateLogicalDevice();
    }

    /// @brief Destroys all the Vulkan objects this class is a wrapper of.
    void VulkanBase::Cleanup() {
        DestroyLogicalDevice();
        DestroyDebugMessenger();
        DestroySurface();
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
        appInfo.applicationVersion = VK_MAKE_API_VERSION(1, 0, 0, 0);
        appInfo.pEngineName = "Flashlight Engine";
        appInfo.engineVersion = VK_MAKE_API_VERSION(1, 0, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        auto requiredExtensions = GetRequiredInstanceExtensions();

        createInfo.enabledExtensionCount = static_cast<u32>(requiredExtensions.size());
        createInfo.ppEnabledExtensionNames = requiredExtensions.data();

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
#if defined(FL_DEBUG)
        createInfo.enabledLayerCount = static_cast<u32>(m_ValidationLayers.size());
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

        CheckRequiredInstanceExtensionsSupport();

        std::cout << glfwGetVersionString() << std::endl;
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


        if (vkCreateDebugUtilsMessengerEXT(m_Vulkan.Instance, &createInfo, nullptr, &m_Vulkan.DebugMessenger) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create debug messenger.");
        }
#endif
    }

    /// @brief Destroys the debug messenger.
    void VulkanBase::DestroyDebugMessenger() const {
#if defined(FL_DEBUG)
        vkDestroyDebugUtilsMessengerEXT(m_Vulkan.Instance, m_Vulkan.DebugMessenger, nullptr);
#endif
    }

    /// @brief Creates the window surface.
    void VulkanBase::CreateSurface() {
        if (glfwCreateWindowSurface(m_Vulkan.Instance, m_Window.GetNativeWindow(), nullptr, &m_Vulkan.Surface) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create window surface.");
        }
    }

    /// @brief Destroys the window surface.
    void VulkanBase::DestroySurface() const {
        vkDestroySurfaceKHR(m_Vulkan.Instance, m_Vulkan.Surface, nullptr);
    }

    /// @brief Picks a Vulkan physical device.
    void VulkanBase::PickPhysicalDevice() {
        u32 deviceCount = 0;
        vkEnumeratePhysicalDevices(m_Vulkan.Instance, &deviceCount, nullptr);

        if (deviceCount == 0) {
            throw std::runtime_error("Failed to find GPUs with Vulkan support.");
        }

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(m_Vulkan.Instance, &deviceCount, devices.data());

        std::multimap<i32, VkPhysicalDevice> candidates;

        for (const auto &device : devices) {
            i32 score = RateDeviceSuitability(device);
            candidates.insert(std::make_pair(score, device));
        }

        if (candidates.rbegin()->first > 0) {
            m_Vulkan.PhysicalDevice = candidates.rbegin()->second;
        } else {
            throw std::runtime_error("Failed to find a suitable GPU");
        }

        // Stores the properties for the chosen physical device.
        DeviceProperties = GetDeviceProperties(m_Vulkan.PhysicalDevice);

        std::cout << "GPU Informations : " << '\n'
        << '\t' << "GPU Name : " << DeviceProperties.deviceName << '\n'
        << '\t' <<"GPU Driver Version : " << DeviceProperties.driverVersion << '\n'
        << '\t' << "GPU Vulkan API Version : " << DeviceProperties.apiVersion << '\n' << std::endl;
    }

    /// @brief Creates the Vulkan logical device.
    void VulkanBase::CreateLogicalDevice() {
        QueueFamilyIndices indices = FindQueueFamilies(m_Vulkan.PhysicalDevice);

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<u32> uniqueQueueFamilies = {indices.GraphicsFamily, indices.PresentFamily};

        float queuePriority = 1.0f;
        for (u32 queueFamily : uniqueQueueFamilies) {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        VkPhysicalDeviceFeatures deviceFeatures{};

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        createInfo.queueCreateInfoCount = static_cast<u32>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();

        createInfo.pEnabledFeatures = &deviceFeatures;

        createInfo.enabledExtensionCount = static_cast<u32>(m_DeviceExtensions.size());
        createInfo.ppEnabledExtensionNames = m_DeviceExtensions.data();

        const auto availableDeviceExtensions = GetAvailableDeviceExtensions(m_Vulkan.PhysicalDevice);
        std::cout << "Available device extensions : \n";
        for (const auto &extension : availableDeviceExtensions) {
            std::cout << '\t' << extension.extensionName << '\n';
        }

        std::cout << std::endl;

        std::cout << "Required device extensions : \n";
        for (const auto &extension : m_DeviceExtensions) {
            std::cout << '\t' << extension << '\n';
        }

        std::cout << std::endl;

#if FL_DEBUG
        createInfo.enabledLayerCount = static_cast<u32>(m_ValidationLayers.size());
        createInfo.ppEnabledLayerNames = m_ValidationLayers.data();
#else
        createInfo.enabledLayerCount = 0;
#endif

        if (vkCreateDevice(m_Vulkan.PhysicalDevice, &createInfo, nullptr, &m_Vulkan.LogicalDevice) != VK_SUCCESS) {
            throw std::runtime_error("failed to create logical device!");
        }

        vkGetDeviceQueue(m_Vulkan.LogicalDevice, indices.GraphicsFamily, 0, &m_Vulkan.GraphicsQueue);
        vkGetDeviceQueue(m_Vulkan.LogicalDevice, indices.PresentFamily, 0, &m_Vulkan.PresentQueue);
    }

    /// @brief Destroys the Vulkan logical device.
    void VulkanBase::DestroyLogicalDevice() const {
        vkDestroyDevice(m_Vulkan.LogicalDevice, nullptr);
    }

    /// @brief Returns the required instance extension for the application to work.
    ///
    /// @returns A std::vector containing the names of the required instance extensions.
    std::vector<const char*> VulkanBase::GetRequiredInstanceExtensions() const noexcept {
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

    /// @brief Returns the available instance extensions.
    ///
    /// @returns A std::vector of the instance extensions properties.
    std::vector<VkExtensionProperties> VulkanBase::GetAvailableInstanceExtensions() const noexcept {
        u32 extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> extensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

        return extensions;
    }


    /// @brief Checks if the required instance extensions are avaiable.
    ///
    /// @returns A boolean telling if all of the required instance extensions are available.
    void VulkanBase::CheckRequiredInstanceExtensionsSupport() const {
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
                throw std::runtime_error("Missing required instance extension.");
            }
        }

        std::cout << std::endl;
    }

    /// @brief Checks if validations layers are supported.
    ///
    /// @returns A boolean telling if validation layers are supported.
    bool VulkanBase::CheckValidationLayerSupport() const noexcept {
        u32 layerCount;
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

        bool extensionsSupported = CheckDeviceExtensionSupport(physicalDevice);

        bool swapChainAdequate = false;
        if (extensionsSupported) {
            SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(physicalDevice);
            swapChainAdequate = !swapChainSupport.Formats.empty() && !swapChainSupport.PresentModes.empty();
        }

        return indices.IsComplete() && extensionsSupported && swapChainAdequate;
    }

    /// @brief Checks if required device extensions are supported by the provided device.
    ///
    /// @param physicalDevice The physical device to check.
    ///
    /// @returns A boolean telling if required device extensions are available.
    bool VulkanBase::CheckDeviceExtensionSupport(VkPhysicalDevice physicalDevice) const noexcept {

        std::vector<VkExtensionProperties> availableExtensions = GetAvailableDeviceExtensions(physicalDevice);

        std::set<std::string> requiredExtensions(m_DeviceExtensions.begin(), m_DeviceExtensions.end());

        for (const auto &extension : availableExtensions) {
            requiredExtensions.erase(extension.extensionName);
        }

        return requiredExtensions.empty();
    }

    /// @brief Returns available extensions for the provided device.
    ///
    /// @param physicalDevice The physical device to check.
    ///
    /// @returns A std::vector of the extensions supported by the device.
    std::vector<VkExtensionProperties> VulkanBase::GetAvailableDeviceExtensions(VkPhysicalDevice physicalDevice) const noexcept {
        u32 extensionCount = 0;
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableExtensions.data());

        return availableExtensions;
    }

    /// @brief Rates devices with a score to help select it.
    ///
    /// @param physicalDevice The physical device to rate.
    ///
    /// @returns The score of the provided device.
    i32 VulkanBase::RateDeviceSuitability(VkPhysicalDevice physicalDevice) const noexcept {
        if (!IsDeviceSuitable(physicalDevice)) {
            // Devices that aren't suitable are scored 0.
            return 0;
        }

        i32 score = 0;

        auto deviceProperties = GetDeviceProperties(physicalDevice);

        // Discrete GPUs have a significant performance advantage.
        if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            score += 1000;
        }

        // Maximum possible size of textures affects graphics quality.
        score += static_cast<i32>(deviceProperties.limits.maxImageDimension2D);

        return score;
    }

    /// @brief Retrieve queue families from the physical device and returns them.
    ///
    /// @param physicalDevice The physical device to retrieve the queue families from.
    ///
    /// @returns A QueueFamilyIndices struct.
    VulkanBase::QueueFamilyIndices VulkanBase::FindQueueFamilies(VkPhysicalDevice physicalDevice) const noexcept {
        QueueFamilyIndices indices;

        u32 queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

        i32 i = 0;
        for (const auto &queueFamily : queueFamilies) {
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.GraphicsFamily = i;
                indices.GraphicsFamilyHasValue = true;
            }

            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, m_Vulkan.Surface, &presentSupport);

            if (presentSupport) {
                indices.PresentFamily = i;
                indices.PresentFamilyHasValue = true;
            }

            if (indices.IsComplete()) {
                break;
            }

            i++;
        }

        return indices;
    }

    /// @brief Retrieves the swap chain support details for the provided device.
    ///
    /// @param physicalDevice The device to query the details from.
    ///
    /// @returns A SwapChainSupportDetails struct containing the details.
    VulkanBase::SwapChainSupportDetails VulkanBase::QuerySwapChainSupport(VkPhysicalDevice physicalDevice) const noexcept {
        SwapChainSupportDetails details;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, m_Vulkan.Surface, &details.Capabilities);

        u32 formatCount = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_Vulkan.Surface, &formatCount, nullptr);

        if (formatCount != 0) {
            details.Formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_Vulkan.Surface, &formatCount, details.Formats.data());
        }

        u32 presentModeCount = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_Vulkan.Surface, &presentModeCount, nullptr);

        if (presentModeCount != 0) {
            details.PresentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_Vulkan.Surface, &presentModeCount, details.PresentModes.data());
        }

        return details;
    }
    
}

}