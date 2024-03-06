#include "Application.hpp"

#include "Core/Window.hpp"
#include "OpenGLRenderer/GLLoader.hpp"

void Application::Run() {
    Start();
    MainLoop();
    End();
}

void Application::Start() {
    WindowProperties properties {800, 600, "Engine Window"};
    m_Window = std::make_unique<Window>(properties);

    GLLoader::LoadGLLoader(properties);
}

void Application::MainLoop() {
    while (!m_Window->ShouldClose()) {
        m_Window->Update();
    }
}

void Application::End() {
    m_Window->Terminate();
}
