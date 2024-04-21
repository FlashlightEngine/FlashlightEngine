#pragma once

#if defined(VK_USE_PLATFORM_WIN32)

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <vulkan/vulkan.h>

#endif

namespace Flashlight {

namespace VulkanWrapper {

#if defined(VK_USE_PLATFORM_WIN32)    
    using VulkanLibHandle = HMODULE;
#elif defined(VK_USE_PLATFORM_LINUX)
    using VulkanLibHandle = void*;
#endif

    class VulkanLoader {
    public:
        VulkanLoader() = delete;
        VulkanLoader(VulkanLoader&) = delete;
        VulkanLoader &operator=(VulkanLoader&) = delete;
        VulkanLoader(VulkanLoader&&) = delete;
        VulkanLoader &operator=(VulkanLoader&&) = delete;
        ~VulkanLoader() = delete;

        static void LoadVulkanLib();
        static void LoadExportedFunctions();
        static void LoadGlobalFunctions();

#define VK_EXPORTED_FUNCTION( fun ) static extern PFN_##fun fun;
#include "ExportedFunctions.hpp"
    
#define VK_GLOBAL_FUNCTION( fun ) static extern PFN_##fun fun;
#include "GlobalFunctions.hpp"

    private:
        static VulkanLibHandle m_VulkanLibrary;
    };
    
}

}
