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
    Application();
    virtual ~Application() = default;

    void Run();
    virtual void Update() = 0;
    virtual void Render() = 0;

    static Application& Get() noexcept;
    Window GetWindow() const noexcept;

protected:
    std::unique_ptr<Window> m_Window = Window::Create(WindowProperties{800, 600, "Flashlight Window"});
    Renderer m_Renderer{*m_Window};

private:
    bool m_Running = true;
    static Application *s_Application;
};

std::unique_ptr<Application> CreateApp();

}