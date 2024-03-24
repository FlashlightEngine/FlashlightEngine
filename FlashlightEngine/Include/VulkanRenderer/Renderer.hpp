/* Copyright (C) 2024 Jean "Pixfri" Letessier (jean.letessier@protonmail.com)
 * This file is part of "FlashLight Engine"
 * For conditions of distribution and use, see copyright notice in FlashLightEngine.hpp
 *
 * Renderer.hpp - The definition of the Render class.
 * This header file contains the definition of the Renderer class, which handles every object concerning rendering with Vulkan.
 */

#pragma once

#include "Core/Window.hpp"
#include "VulkanObjects/VulkanBase.hpp"

namespace Flashlight {

class Renderer {
public:
    Renderer(const Window &window);
    ~Renderer() = default;

    static std::unique_ptr<Renderer> Create(const Window& window);

private:
    Window m_Window;
    std::unique_ptr<VulkanBase> m_Vulkan;

    void Init();
};

}
