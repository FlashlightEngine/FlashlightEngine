#include "TestApplication.hpp"


bool TestApplication::Init(const Flashlight::WindowProperties& windowProperties) {
    if (!Application::Init(windowProperties)) {
        Flashlight::Log::AppFatal({0x00, 0x01}, "Failed to initialize application base.");
    }
    
    m_IsRunning = true;

    return true;
}

void TestApplication::Update() {
    Application::Update();
}

void TestApplication::Render() {
    m_Renderer->BeginRendering(*m_Window);
    m_Renderer->EndRendering();
}

void TestApplication::Cleanup() {
}