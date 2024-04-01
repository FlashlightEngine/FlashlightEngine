/* Copyright (C) 2024 Jean "Pixfri" Letessier (jean.letessier@protonmail.com)
 * This file is part of "FlashLight Engine"
 * For conditions of distribution and use, see copyright notice in FlashLightEngine.hpp
 *
 * File : Renderer.cpp
 * Description : This file contains definitions of the methods from the Flashlight::Renderer class.
 */

#include "FlashlightEngine/VulkanRenderer/VulkanRenderer.hpp"

#include "FlashlightEngine/VulkanRenderer/Renderer.hpp"
#include "FlashlightEngine/VulkanRenderer/VulkanWrapper/VulkanBase.hpp"

namespace Flashlight {

/// @ingroup VulkanRenderer
/// @class Flashlight::Renderer
/// @brief The class that manages the Vulkan rendering engine.

/// @brief Initializes every object in the Renderer.
void Renderer::Init() {
    m_SwapChain = VulkanSwapChain::Create(m_Window, m_VulkanBase);
}

}