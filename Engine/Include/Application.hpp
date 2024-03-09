#pragma once

#include "Core/Window.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class Application {
public:
    void Run();

private:
    std::unique_ptr<Window> m_Window = Window::Create();

    void Start();
    void Update();
};