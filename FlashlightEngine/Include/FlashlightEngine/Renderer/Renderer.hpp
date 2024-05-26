#pragma once

#include "FlashlightEngine/VulkanWrapper/VulkanWrapper.hpp"

namespace Flashlight {

class Renderer {
public:
    Renderer(Window &window);
    Renderer(const Renderer &) = delete;
    Renderer(Renderer &&) = delete;
    ~Renderer() = default;

    VulkanWrapper::VulkanGraphicsPipeline CreateGraphicsPipeline();

private:
    VulkanWrapper::VulkanInstance m_Instance;

#ifdef FL_DEBUG
    VulkanWrapper::VulkanDebugMessenger m_DebugMessenger;
#endif

    VulkanWrapper::VulkanWindowSurface m_WindowSurface;
    VulkanWrapper::VulkanDevice m_Device;
    VulkanWrapper::VulkanSwapChain m_SwapChain;
};

}
