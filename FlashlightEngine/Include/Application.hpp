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