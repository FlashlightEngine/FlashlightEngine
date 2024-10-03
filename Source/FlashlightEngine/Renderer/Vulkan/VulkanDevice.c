// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.h

#include "FlashlightEngine/Renderer/Vulkan/VulkanDevice.h"

#include "FlashlightEngine/Core/Logger.h"
#include "FlashlightEngine/Core/FlString.h"
#include "FlashlightEngine/Core/FlMemory.h"

#include "FlashlightEngine/Containers/DArray.h"

typedef struct FlVulkanPhysicalDeviceRequirements {
    FlBool8 Graphics;
    FlBool8 Present;
    FlBool8 Compute;
    FlBool8 Transfer;

    // DArray
    const char** DeviceExtensionNames;

    FlBool8 SamplerAnisotropy;
    FlBool8 DiscreteGPU;
} FlVulkanPhysicalDeviceRequirements;

typedef struct FlVulkanPhysicalDeviceQueueFamilyInfo {
    FlUInt32 GraphicsFamilyIndex;
    FlUInt32 PresentFamilyIndex;
    FlUInt32 ComputeFamilyIndex;
    FlUInt32 TransferFamilyIndex;
} FlVulkanPhysicalDeviceQueueFamilyInfo;

FlBool8 flSelectPhysicalDevice(FlVulkanContext* context);
FlBool8 flPhysicalDeviceCheckRequirements(
    VkPhysicalDevice device,
    VkSurfaceKHR surface,
    const VkPhysicalDeviceProperties* properties,
    const VkPhysicalDeviceFeatures* features,
    const FlVulkanPhysicalDeviceRequirements* requirements,
    FlVulkanPhysicalDeviceQueueFamilyInfo* outQueueFamilyInfo,
    FlVulkanSwapchainSupport* outSwapchainSupport
);

FlBool8 flVulkanDeviceCreate(FlVulkanContext* context) {
    if (!flSelectPhysicalDevice(context)) {
        return FALSE;
    }

    FL_LOG_INFO("Creating Vulkan logical device...")
    // NOTE: Do not create additional queues for shared indices.
    FlBool8 presentSharesGraphicsQueue = context->Device.GraphicsQueueIndex == context->Device.PresentQueueIndex;
    FlBool8 transferSharesGraphicsQueue = context->Device.GraphicsQueueIndex == context->Device.TransferQueueIndex;
    FlUInt32 indexCount = 1;

    if (!presentSharesGraphicsQueue) {
        indexCount++;
    }

    if (!transferSharesGraphicsQueue) {
        indexCount++;
    }

    FlUInt32 indices[indexCount];
    FlUInt8 index = 0;
    indices[index++] = context->Device.GraphicsQueueIndex;

    if (!presentSharesGraphicsQueue) {
        indices[index++] = context->Device.PresentQueueIndex;
    }

    if (!transferSharesGraphicsQueue) {
        indices[index++] = context->Device.TransferQueueIndex;
    }

    VkDeviceQueueCreateInfo queueCreateInfos[indexCount];
    for (FlUInt32 i = 0; i < indexCount; ++i) {
        queueCreateInfos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfos[i].queueFamilyIndex = indices[i];
        queueCreateInfos[i].queueCount = 1;
        queueCreateInfos[i].flags = 0;
        queueCreateInfos[i].pNext = 0;
        const FlFloat32 queuePriority = 1.f;
        queueCreateInfos[i].pQueuePriorities = &queuePriority;
    }

    // Request device features.
    // TODO: Should be config driven.
    VkPhysicalDeviceFeatures deviceFeatures = {};
    deviceFeatures.samplerAnisotropy = VK_TRUE;

    VkDeviceCreateInfo deviceCreateInfo = {VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
    deviceCreateInfo.queueCreateInfoCount = indexCount;
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfos;
    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
    deviceCreateInfo.enabledExtensionCount = 1;
    const char* extensionNames = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
    deviceCreateInfo.ppEnabledExtensionNames = &extensionNames;

    // Deprecated and ignored, so pass nothing.
    deviceCreateInfo.enabledLayerCount = 0;
    deviceCreateInfo.ppEnabledLayerNames = 0;

    VK_CHECK(vkCreateDevice(
        context->Device.PhysicalDevice, 
        &deviceCreateInfo, 
        context->Allocator, 
        &context->Device.LogicalDevice));

    FL_LOG_INFO("Vulkan logical device created.")

    vkGetDeviceQueue(context->Device.LogicalDevice, context->Device.GraphicsQueueIndex, 0, &context->Device.GraphicsQueue);
    vkGetDeviceQueue(context->Device.LogicalDevice, context->Device.PresentQueueIndex, 0, &context->Device.PresentQueue);
    vkGetDeviceQueue(context->Device.LogicalDevice, context->Device.TransferQueueIndex, 0, &context->Device.TransferQueue);

    FL_LOG_INFO("Vulkan queues obtained.")

    return TRUE;
}

void flVulkanDeviceDestroy(FlVulkanContext* context) {
    // Unset queues.
    context->Device.GraphicsQueue = VK_NULL_HANDLE;
    context->Device.PresentQueue = VK_NULL_HANDLE;
    context->Device.TransferQueue = VK_NULL_HANDLE;

    FL_LOG_INFO("Destroying Vulkan logical device...")
    if (context->Device.LogicalDevice) {
        vkDestroyDevice(context->Device.LogicalDevice, context->Allocator);
        context->Device.LogicalDevice = VK_NULL_HANDLE;
    }

    // Physical devices are not destroyed.
    FL_LOG_INFO("Releasing physical device resources...")
    context->Device.PhysicalDevice = VK_NULL_HANDLE;

    if (context->Device.SwapchainSupport.Formats) {
        flFree(
            context->Device.SwapchainSupport.Formats,
            sizeof(VkSurfaceFormatKHR) * context->Device.SwapchainSupport.FormatCount,
            FlMemoryTagRenderer
        );
        context->Device.SwapchainSupport.Formats = 0;
        context->Device.SwapchainSupport.FormatCount = 0;
    }

    if (context->Device.SwapchainSupport.PresentModes) {
        flFree(
            context->Device.SwapchainSupport.PresentModes,
            sizeof(VkPresentModeKHR) * context->Device.SwapchainSupport.PresentModeCount,
            FlMemoryTagRenderer
        );
        context->Device.SwapchainSupport.PresentModes = 0;
        context->Device.SwapchainSupport.PresentModeCount = 0;
    }

    flZeroMemory(
        &context->Device.SwapchainSupport.Capabilities,
        sizeof(context->Device.SwapchainSupport.Capabilities)
    );

    context->Device.GraphicsQueueIndex = UINT32_MAX;
    context->Device.PresentQueueIndex = UINT32_MAX;
    context->Device.TransferQueueIndex = UINT32_MAX;
}

FlBool8 flSelectPhysicalDevice(FlVulkanContext* context) {
    FlUInt32 physicalDeviceCount;
    VK_CHECK(vkEnumeratePhysicalDevices(context->Instance, &physicalDeviceCount, 0))
    if (physicalDeviceCount == 0) {
        FL_LOG_FATAL("No devices which support Vulkan were found.")
        return FALSE;
    }
    
    VkPhysicalDevice physicalDevices[physicalDeviceCount];
    VK_CHECK(vkEnumeratePhysicalDevices(context->Instance, &physicalDeviceCount, physicalDevices))

    for (FlUInt32 i = 0; i < physicalDeviceCount; ++i) {
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(physicalDevices[i], &properties);

        VkPhysicalDeviceFeatures features;
        vkGetPhysicalDeviceFeatures(physicalDevices[i], &features);

        VkPhysicalDeviceMemoryProperties memoryProperties;
        vkGetPhysicalDeviceMemoryProperties(physicalDevices[i], &memoryProperties);

        // TODO: These requirements should be driven by engine
        // configuration.
        FlVulkanPhysicalDeviceRequirements requirements = {};
        requirements.Graphics = TRUE;
        requirements.Present = TRUE;
        requirements.Transfer = TRUE;
        // NOTE: Enable if compute will be required.
        // requirements.Compute = TRUE;
        requirements.SamplerAnisotropy = TRUE;
        requirements.DiscreteGPU = TRUE;
        requirements.DeviceExtensionNames = flDArrayCreate(const char*);
        flDArrayPush(requirements.DeviceExtensionNames, &VK_KHR_SWAPCHAIN_EXTENSION_NAME);

        FlVulkanPhysicalDeviceQueueFamilyInfo queueInfo = {};

        FlBool8 result = flPhysicalDeviceCheckRequirements(
            physicalDevices[i],
            context->Surface,
            &properties,
            &features,
            &requirements,
            &queueInfo,
            &context->Device.SwapchainSupport
        );

        if (result) {
            FL_LOG_INFO("Selected device: '%s'.", properties.deviceName);
            // GPU type.
            switch (properties.deviceType) {
            default:
            case VK_PHYSICAL_DEVICE_TYPE_OTHER:
                FL_LOG_INFO("\t- GPU Type: Unknown.")
                break;
            case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
                FL_LOG_INFO("\t- GPU Type: Integrated")
                break;
            case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
                FL_LOG_INFO("\t- GPU Type: Dedicated")
                break;
            case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
                FL_LOG_INFO("\t- GPU Type: Virtual")
                break;
            case VK_PHYSICAL_DEVICE_TYPE_CPU:
                FL_LOG_INFO("\t- GPU Type: CPU")
                break;
            }                       

            // This part is meant to show the driver version correctly because
            // vendors couldn't agree to just use one way.

            // 4318 : NVIDIA
            if (properties.vendorID == 4318) {
                FL_LOG_INFO("\t- GPU Driver Version: %d.%d.%d.%d",
                    (properties.driverVersion >> 22) & 0x3FF,
                    (properties.driverVersion >> 14) & 0x0FF,
                    (properties.driverVersion >> 6) & 0x0FF,
                    (properties.driverVersion) & 0x003F)
            }

#ifdef FL_PLATFORM_WINDOWS
            // 0x8086 : Intel, only differs on Windows for some reason.
            else if (properties.vendorID == 0x8086) {
                FL_LOG_INFO("\t- GPU Driver Version: %d.%d",
                    (properties.driverVersion >> 14),
                    (properties.driverVersion) & 0x3FFF);
            }
#endif
            // For other vendors, use Vulkan macros.
            else {
                FL_LOG_INFO("\t- GPU Driver Version: %d.%d.%d",
                    VK_API_VERSION_MAJOR(properties.driverVersion),
                    VK_API_VERSION_MINOR(properties.driverVersion),
                    VK_API_VERSION_PATCH(properties.driverVersion))
            }

            // Vulkan API version.
            FL_LOG_INFO("\t- GPU Vulkan API Version: %d.%d.%d",
                VK_API_VERSION_MAJOR(properties.apiVersion),
                VK_API_VERSION_MINOR(properties.apiVersion),
                VK_API_VERSION_PATCH(properties.apiVersion))

            // Memory information.
            for (FlUInt32 j = 0; j < memoryProperties.memoryHeapCount; ++j) {
                FlFloat32 memorySizeGib = (((FlFloat32)memoryProperties.memoryHeaps[j].size) / 1024.f / 1024.f / 1024.f);
                if (memoryProperties.memoryHeaps[j].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) {
                    FL_LOG_INFO("\t- GPU Local Memory: %.2f GiB", memorySizeGib)
                } else {
                    FL_LOG_INFO("\t- Shared System Memory: %.2f GiB", memorySizeGib)
                }
            }

            context->Device.PhysicalDevice = physicalDevices[i];
            context->Device.GraphicsQueueIndex = queueInfo.GraphicsFamilyIndex;
            context->Device.PresentQueueIndex = queueInfo.PresentFamilyIndex;
            context->Device.TransferQueueIndex = queueInfo.TransferFamilyIndex;
            // NOTE: Set compute queue family index if needed.

            // Keep a copy of properties, features and memory info for later use.
            context->Device.Properties = properties;
            context->Device.Features = features;
            context->Device.MemoryProperties = memoryProperties;
            break;
        }
    }
    
    // Ensure a device was selected.
    if (!context->Device.PhysicalDevice) {
        FL_LOG_ERROR("No physical devices which meet the requirements were found.")
        return FALSE;
    }

    FL_LOG_INFO("Physical device selected successfully.")
    return TRUE;
}

void flVulkanDeviceQuerySwapchainSupport(
    VkPhysicalDevice physicalDevice, 
    VkSurfaceKHR surface, 
    FlVulkanSwapchainSupport* outSupportInfo) {
    // Surface capabilities.
    VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        physicalDevice, 
        surface, 
        &outSupportInfo->Capabilities))

    // Surface formats.
    VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(
        physicalDevice, 
        surface, 
        &outSupportInfo->FormatCount, 
        0));

    if (outSupportInfo->FormatCount != 0) {
        if (!outSupportInfo->Formats) {
            outSupportInfo->Formats = flAllocate(sizeof(VkSurfaceFormatKHR) * outSupportInfo->FormatCount, FlMemoryTagRenderer);
        }
        VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(
            physicalDevice,
            surface,
            &outSupportInfo->FormatCount,
            outSupportInfo->Formats));
    }

    VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(
        physicalDevice,
        surface,
        &outSupportInfo->PresentModeCount,
        0))

    if (outSupportInfo->PresentModeCount != 0) {
        if (!outSupportInfo->PresentModes) {
            outSupportInfo->PresentModes = flAllocate(sizeof(VkPresentModeKHR) * outSupportInfo->PresentModeCount, FlMemoryTagRenderer);
        }
        VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(
            physicalDevice, 
            surface, 
            &outSupportInfo->PresentModeCount,
            outSupportInfo->PresentModes));
    }
}

FlBool8 flPhysicalDeviceCheckRequirements(
    VkPhysicalDevice device,
    VkSurfaceKHR surface,
    const VkPhysicalDeviceProperties* properties,
    const VkPhysicalDeviceFeatures* features,
    const FlVulkanPhysicalDeviceRequirements* requirements,
    FlVulkanPhysicalDeviceQueueFamilyInfo* outQueueFamilyInfo,
    FlVulkanSwapchainSupport* outSwapchainSupport
) {
    // Evalutate device properties to determine if it meets the needs of our application.
    outQueueFamilyInfo->GraphicsFamilyIndex = UINT32_MAX;
    outQueueFamilyInfo->PresentFamilyIndex = UINT32_MAX;
    outQueueFamilyInfo->ComputeFamilyIndex = UINT32_MAX;
    outQueueFamilyInfo->TransferFamilyIndex = UINT32_MAX;

    // Discrete GPU?
    if (requirements->DiscreteGPU) {
        if (properties->deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            FL_LOG_INFO("Device is not a discrete GPU, and one is required. Skipping.")
            return TRUE;
        }
    }

    FlUInt32 queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, 0);
    VkQueueFamilyProperties queueFamilies[queueFamilyCount];
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies);

    // Look at each queue and see what queues it supports.
    FL_LOG_INFO("Graphics | Present | Compute | Transfer | Name")
    FlUInt8 minTransferScore = 255;
    for (FlUInt32 i = 0; i < queueFamilyCount; ++i) {
        FlUInt32 currentTransferScore = 0;

        // Graphics queue?
        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            outQueueFamilyInfo->GraphicsFamilyIndex = i;
            ++currentTransferScore;
        }

        // Compute queue?
        if (queueFamilies[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {
            outQueueFamilyInfo->ComputeFamilyIndex = i;
            ++currentTransferScore;
        }

        // Transfer queue?
        if (queueFamilies[i].queueFlags & VK_QUEUE_TRANSFER_BIT) {
            // Take the index if it is the current lowest. This increases
            // liklihood that it is a dedicated transfer queue.
            if (currentTransferScore <= minTransferScore) {
                minTransferScore = currentTransferScore;
                outQueueFamilyInfo->TransferFamilyIndex = i;
            }
        }

        // Present queue?
        VkBool32 supportsPresent = VK_FALSE;
        VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &supportsPresent))
        if (supportsPresent) {
            outQueueFamilyInfo->PresentFamilyIndex = i;
        }
    }
    
    // Print out some info about the device.
    FL_LOG_INFO("       %d |       %d |       %d |        %d | %s",
        outQueueFamilyInfo->GraphicsFamilyIndex != UINT32_MAX,
        outQueueFamilyInfo->PresentFamilyIndex != UINT32_MAX,
        outQueueFamilyInfo->ComputeFamilyIndex != UINT32_MAX,
        outQueueFamilyInfo->PresentFamilyIndex != UINT32_MAX,
        properties->deviceName);

    if (
        (!requirements->Graphics || (requirements->Graphics && outQueueFamilyInfo->GraphicsFamilyIndex != UINT32_MAX)) &&
        (!requirements->Present || (requirements->Present && outQueueFamilyInfo->PresentFamilyIndex != UINT32_MAX)) &&
        (!requirements->Compute || (requirements->Compute && outQueueFamilyInfo->ComputeFamilyIndex != UINT32_MAX)) &&
        (!requirements->Transfer || (requirements->Transfer && outQueueFamilyInfo->TransferFamilyIndex != UINT32_MAX))) {
        FL_LOG_INFO("Device meets queue requirements.")
        FL_LOG_TRACE("Graphics Family Index: %i", outQueueFamilyInfo->GraphicsFamilyIndex);
        FL_LOG_TRACE("Present Family Index:  %i", outQueueFamilyInfo->PresentFamilyIndex);
        FL_LOG_TRACE("Compute Family Index:  %i", outQueueFamilyInfo->PresentFamilyIndex);
        FL_LOG_TRACE("Transfer Family Index: %i", outQueueFamilyInfo->TransferFamilyIndex);

        // Query swapchain support.
        flVulkanDeviceQuerySwapchainSupport(device, surface, outSwapchainSupport);

        if (outSwapchainSupport->FormatCount < 1 || outSwapchainSupport->PresentModeCount < 1) {
            if (outSwapchainSupport->Formats) {
                flFree(outSwapchainSupport->Formats, sizeof(VkSurfaceFormatKHR) * outSwapchainSupport->FormatCount, FlMemoryTagRenderer);
            }
            if (outSwapchainSupport->PresentModes) {
                flFree(outSwapchainSupport->PresentModes, sizeof(VkPresentModeKHR) * outSwapchainSupport->PresentModeCount, FlMemoryTagRenderer);
            }
            FL_LOG_INFO("Required swapchain support not present, skipping device.")
            return FALSE;
        }

        // Device extensions.
        if (requirements->DeviceExtensionNames) {
            FlUInt32 availableExtensionCount = 0;
            VkExtensionProperties* availableExtensions = 0;
            VK_CHECK(vkEnumerateDeviceExtensionProperties(device, 0, &availableExtensionCount, 0))

            if (availableExtensionCount != 0) {
                availableExtensions = flAllocate(sizeof(VkExtensionProperties) * availableExtensionCount, FlMemoryTagRenderer);
                VK_CHECK(vkEnumerateDeviceExtensionProperties(device, 0, &availableExtensionCount, availableExtensions))

                FlUInt32 requiredExtensionCount = flDArrayLength(requirements->DeviceExtensionNames);
                for (FlUInt32 i = 0; i < requiredExtensionCount; ++i) {
                    FlBool8 found = FALSE;
                    for (FlUInt32 j = 0; j < availableExtensionCount; ++j) {
                        if (flStringsEqual(requirements->DeviceExtensionNames[i], availableExtensions[j].extensionName)) {
                            found = TRUE;
                            break;
                        }
                    }

                    if (!found) {
                        FL_LOG_INFO("Required extension not found: '%s', skipping device.", requirements->DeviceExtensionNames[i])
                        flFree(availableExtensions, sizeof(VkExtensionProperties) * availableExtensionCount, FlMemoryTagRenderer);
                        return FALSE;
                    }
                }
            }
            
            flFree(availableExtensions, sizeof(VkExtensionProperties) * availableExtensionCount, FlMemoryTagRenderer);
        }

        // Sampler anisotropy.
        if (requirements->SamplerAnisotropy && !features->samplerAnisotropy) {
            FL_LOG_INFO("Device does not support samplerAnisotropy, skipping.")
            return FALSE;
        }

        // Device meets all requirements.
        return TRUE;
    }

    return FALSE;
}
