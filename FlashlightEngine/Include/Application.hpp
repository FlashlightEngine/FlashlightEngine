/* Copyright (C) 2024 Jean "Pixfri" Letessier (jean.letessier@protonmail.com)
 * This file is part of "FlashLight Engine"
 * For conditions of distribution and use, see copyright notice in FlashLightEngine.hpp
 *
 * Application.hpp - This file contains the definition of the Application class.
 * The application class is the boilerplate for an Application in the Flashlight Engine.
 */

#pragma once

#include "Core/Window.hpp"
#include "VulkanRenderer/Renderer.hpp"

namespace Flashlight {

class Application {
public:
    void Run();

private:
    std::unique_ptr<Window> m_Window = Window::Create();
    std::unique_ptr<Renderer> m_Renderer = Renderer::Create(*m_Window);

    void Start();
    void Update();
};

}