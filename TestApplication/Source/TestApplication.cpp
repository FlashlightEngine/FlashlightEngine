#include "TestApplication.hpp"


bool TestApplication::Init(const Flashlight::WindowProperties& windowProperties,
                           const Flashlight::DebugLevel& debugLevel) {
    if (!Application::Init(windowProperties, debugLevel)) {
        Flashlight::Log::AppFatal({0x00, 0x01}, "Failed to initialize application base.");
    }

    m_IsRunning = true;

    return true;
}

void TestApplication::Update() {
    Application::Update();
}

void TestApplication::Render() {
    m_Renderer->BeginFrame();
}

void TestApplication::Cleanup() {
    Application::Cleanup();
}
