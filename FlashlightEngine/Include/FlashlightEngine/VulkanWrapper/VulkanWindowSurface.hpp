#pragma once

#include "VulkanInstance.hpp"

#include "FlashlightEngine/Core/Window.hpp"

namespace Flashlight {

namespace VulkanWrapper {

    class VulkanWindowSurface {
    public:
        inline VulkanWindowSurface();
        VulkanWindowSurface(VulkanWindowSurface &) = delete;
        VulkanWindowSurface(VulkanWindowSurface &&) = delete;
        inline ~VulkanWindowSurface();


        inline bool IsValid() const noexcept;
        inline VkSurfaceKHR GetSurface() const noexcept;

        inline void Create(const VulkanInstance &instance, Window &window);

    private:
        VkInstance m_Instance;
        VkSurfaceKHR m_Handle;
        
        inline void Destroy() const;
    };

#include "VulkanWindowSurface.inl"
    
}

}
