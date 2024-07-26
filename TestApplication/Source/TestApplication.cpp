#include "TestApplication.hpp"


bool TestApplication::Init() {
    m_IsRunning = true;

    return true;
}

void TestApplication::Update() {
    ProcessInput();
}

void TestApplication::Render() {
}

void TestApplication::Cleanup() {
}

void TestApplication::ProcessInput() {
    if (m_Window->GetKeyState(Flashlight::Keys::Escape) == Flashlight::KeyState::Pressed) {
        Close();
    }
}
