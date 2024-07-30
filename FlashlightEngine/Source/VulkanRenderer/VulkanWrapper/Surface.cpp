/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Surface.cpp
 * Description : Definitions of methods from the Surface class.
 */
#include <FlashlightEngine/VulkanRenderer/VulkanWrapper/Surface.hpp>

#include <SDL_vulkan.h>

namespace Flashlight::VulkanRenderer::VulkanWrapper {
    Surface::Surface(const std::shared_ptr<Instance>& instance, const Window& window) : m_Instance(instance) {
        Log::EngineTrace("Creating Vulkan window surface...");
        SDL_Vulkan_CreateSurface(window.GetNativeWindow(), m_Instance->GetInstance(), &m_Surface);
        Log::EngineTrace("Vulkan window surface created.");
    }

    Surface::~Surface() {
        if (m_Surface) {
            Log::EngineTrace("Destroying Vulkan surface.");
            vkDestroySurfaceKHR(m_Instance->GetInstance(), m_Surface, nullptr);
        }
    }

    Surface::Surface(Surface&& other) noexcept {
        m_Surface = other.m_Surface;
        m_Instance = std::move(other.m_Instance);

        other.m_Surface = VK_NULL_HANDLE;
    }
    
    Surface& Surface::operator=(Surface&& other) noexcept {
        m_Surface = other.m_Surface;
        m_Instance = std::move(other.m_Instance);

        other.m_Surface = VK_NULL_HANDLE;
        
        return *this;
    }
}
