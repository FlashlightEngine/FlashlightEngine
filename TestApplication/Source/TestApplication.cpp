#include "TestApplication.hpp"

#include "FlashlightEngine/Core/Logger.hpp"

bool TestApplication::Init() {
    m_Renderer = std::make_unique<Flashlight::Renderer>();

    m_IsRunning = true;

    return true;
}

void TestApplication::Update() {
    Close();
}

void TestApplication::Render() {

}

void TestApplication::Cleanup() {
    Flashlight::Log::AppInfo("Quitting.");
}
