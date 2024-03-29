/* Copyright (C) 2024 Jean "Pixfri" Letessier (jean.letessier@protonmail.com)
 * This file is part of "FlashLight Engine"
 * For conditions of distribution and use, see copyright notice in FlashLightEngine.hpp
 *
 * Renderer.cpp - Definitions of methods from the Renderer class.
 * This file contains definitions of the methods from the Flashlight::Renderer class.
 */

#include "FlashlightEngine/VulkanRenderer/Renderer.hpp"
#include "FlashlightEngine/VulkanRenderer/VulkanObjects/VulkanBase.hpp"

namespace Flashlight {

/// @ingroup VulkanRenderer
/// @class Flashlight::Renderer
/// @brief The class that manages the Vulkan rendering engine.

/// @brief The constructor of the Renderer class. Initializes every object used by the renderer.
///
/// @param window the window of the application using the renderer.
Renderer::Renderer(const Window &window) : m_Window(window) {
    Init();
}

/// @brief Initializes every object in the Renderer.
void Renderer::Init() {
    m_VulkanRenderer = VulkanBase::Create(m_Window);
}

/// @brief Creates a new Renderer object and returns a std::unique_pointer of it.
///
/// @param window The window of the application using the renderer.
///
/// @returns A std::unique_pointer to the newly created Renderer object.
std::unique_ptr<Renderer> Renderer::Create(const Window& window) {
    return std::make_unique<Renderer>(window);
}
}