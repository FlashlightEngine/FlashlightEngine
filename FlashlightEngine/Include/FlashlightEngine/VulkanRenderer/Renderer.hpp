/* Copyright (C) 2024 Jean "Pixfri" Letessier (jean.letessier@protonmail.com)
 * This file is part of "FlashLight Engine"
 * For conditions of distribution and use, see copyright notice in FlashLightEngine.hpp
 *
 * File : Renderer.hpp
 * Description : This header file contains the definition of the Renderer class, which handles every object concerning rendering with Vulkan.
 */

#pragma once

#include "VulkanWrapper/VulkanInstance.hpp"
#include "VulkanWrapper/VulkanDebugMessenger.hpp"
#include "VulkanWrapper/VulkanWindowSurface.hpp"
#include "VulkanWrapper/VulkanDevice.hpp"
#include "VulkanWrapper/VulkanSwapChain.hpp"

#include "FlashlightEngine/Core/Window.hpp"


namespace Flashlight {

class Renderer {
public:
    inline Renderer(const Window &window);
    ~Renderer() = default;

    inline static std::unique_ptr<Renderer> Create(const Window& window);

private:
    Window m_Window;
    VulkanWrapper::VulkanInstance m_Instance;
    VulkanWrapper::VulkanDebugMessenger m_DebugMessenger;
    VulkanWrapper::VulkanWindowSurface m_WindowSurface;
    VulkanWrapper::VulkanDevice m_Device;
    VulkanWrapper::VulkanSwapChain m_SwapChain;

    void Init();
};

#include "Renderer.inl"

}
