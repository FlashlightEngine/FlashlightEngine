#include "FlashlightEngine/VulkanRenderer/VulkanWrapper/VulkanLoader.hpp"

#include "FlashlightEngine/Core/Logger.hpp"

#include <vulkan/vulkan.h>

namespace Flashlight {

namespace VulkanWrapper {

#define VK_EXPORTED_FUNCTION( fun ) PFN_##fun fun;
#include "FlashlightEngine/VulkanRenderer/VulkanWrapper/ExportedFunctions.hpp"
    
#define VK_GLOBAL_FUNCTION( fun ) PFN_##fun fun;
#include "FlashlightEngine/VulkanRenderer/VulkanWrapper/GlobalFunctions.hpp"
    
    void VulkanLoader::LoadVulkanLib() {
#if defined(VK_USE_PLATFORM_WIN32)
        m_VulkanLibrary = LoadLibrary("vulkan-1.dll");
#elif defined(VK_USE_PLATFORM_LINUX)
        m_VulkanLibrary = dlopen("libvulkan.so.1", RTLD_NOW);
#endif

        if (m_VulkanLibrary == nullptr) {
            Log::EngineError("Failed to load Vulkan library.");
        }
    }

    void VulkanLoader::LoadExportedFunctions() {
#if defined(VK_USE_PLATFORM_WIN32)
#define LoadProcAddress GetProcAddress
#elif defined(VK_USE_PLATFORM_LINUX)
#define LoadProcAddress dlsym
#endif

#define VK_EXPORTED_FUNCTION( fun )                                         \
        if( !(fun = reinterpret_cast<PFN_##fun>(LoadProcAddress( VulkanLibrary, #fun ))) ) {  \
            Log::EngineError("Could not load exported function : {}", #fun);   \
        }

#include "FlashlightEngine/VulkanRenderer/VulkanWrapper/ExportedFunctions.hpp"
        
    }

    void VulkanLoader::LoadGlobalFunctions() {
#define VK_GLOBAL_FUNCTION( fun )                                           \
        if ( !(fun = reinterpret_cast<PFN_##fun>(vkGetInstanceProcAddr(nullptr, #fun)) ) {   \
            Log::EngineError("Could not load global function : {}", #fun);  \
        }

#include "FlashlightEngine/VulkanRenderer/VulkanWrapper/GlobalFunctions.hpp"
        
    }
    
    }
}
