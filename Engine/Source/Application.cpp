#include "Application.hpp"

void Application::Run() {
    Start();
    Update();
}

void Application::Start() {}

void Application::Update() {
    while (!m_Window->ShouldClose()) {
        m_Window->Update();
    }
}