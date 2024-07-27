/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Surface.cpp
 * Description : Definitions of methods from the Surface class.
 */

#include "FlashlightEngine/Renderer/VulkanWrapper/Surface.hpp"

namespace Flashlight::VulkanWrapper {
    Surface::Surface(const std::shared_ptr<Instance>& instance, const Window& window) : m_Instance(instance) {
        Log::EngineTrace("Creating window surface...");
        
        if (glfwCreateWindowSurface(m_Instance->GetNativeInstance(), window.GetGlfwWindow(), nullptr, &m_Surface)
            != VK_SUCCESS) {
            Log::EngineFatal({0x01, 0x03}, "Failed to create window surface.");
        } else {
            Log::EngineTrace("Window surface created.");
        }
    }
}
