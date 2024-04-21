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


        bool IsValid() const noexcept;
        bool GetHandle() const noexcept;

        inline void Create(VulkanInstance &instance, Window &window);
        inline void Destroy();

    private:
        VulkanInstance m_Instance;
        VkSurfaceKHR m_Handle;

        inline void CreateHandle(VulkanInstance &instance, Window &window);
        inline void DestroyHandle(VulkanInstance &instance, VkSurfaceKHR &surface);
    };

#include "VulkanWindowSurface.inl"
    
}

}
