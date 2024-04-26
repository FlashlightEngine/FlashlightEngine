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
        inline VkSurfaceKHR GetHandle() const noexcept;

        inline void Create(const VulkanInstance &instance, Window &window);
        inline void Destroy() const;

    private:
        VkInstance m_Instance;
        VkSurfaceKHR m_Handle;
    };

#include "VulkanWindowSurface.inl"
    
}

}
