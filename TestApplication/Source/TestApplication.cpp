#include "TestApplication.hpp"


bool TestApplication::Init() {
    m_IsRunning = true;

    return true;
}

void TestApplication::Update() {
    m_Window->Update();

    if (m_Window->ShouldClose()) {
        m_IsRunning = false;
    }
}

void TestApplication::Render() {
}

void TestApplication::Cleanup() {
}
