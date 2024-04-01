/* Copyright (C) 2024 Jean "Pixfri" Letessier (jean.letessier@protonmail.com)
 * This file is part of "FlashLight Engine"
 * For conditions of distribution and use, see copyright notice in FlashLightEngine.hpp
 *
 * File : Application.hpp
 * Description : This file contains the definition of the Application class.
 */

#pragma once

#include "Core/Window.hpp"
#include "VulkanRenderer/VulkanRenderer.hpp"

namespace Flashlight {

class Application {
public:
    Application();
    virtual ~Application() = default;

    void Run();
    virtual void Update() = 0;
    virtual void Render() = 0;

    inline static Application& Get() noexcept;
    inline Window GetWindow() const noexcept;

protected:
    std::unique_ptr<Window> m_Window = Window::Create(WindowProperties{800, 600, "Flashlight Window <Vulkan 1.0>"});
    Renderer m_Renderer{*m_Window};

private:
    bool m_Running = true;
    static Application *s_Application;
};

#include "Application.inl"

}