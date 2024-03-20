#include "Core/Window.hpp"

#include "defines.hpp"
#include "pch.hpp"

namespace Flashlight {

Window::Window(WindowProperties properties) : m_Properties(std::move(properties)) {
    Init();
}

Window::~Window() {
    Close();
}

bool Window::ShouldClose() const noexcept {
    return glfwWindowShouldClose(m_Window);
}

WindowProperties Window::GetProperties() const noexcept {
    return m_Properties;
}

void Window::Update() {
    if (m_GLFWInitialized) {
        glfwPollEvents();
    }
}

std::unique_ptr<Window> Window::Create() {
    return std::make_unique<Window>(WindowProperties()); // Create a window using the default properties.
}

std::unique_ptr<Window> Window::Create(int32 Width, int32 Height, std::string Title) {
    return std::make_unique<Window>(WindowProperties(Width, Height, std::move(Title)));
}

void Window::Init() {
    if (!m_GLFWInitialized) {
        glfwInit();
        m_GLFWInitialized = true;

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        m_Window = glfwCreateWindow(m_Properties.Width, m_Properties.Height, m_Properties.Title.c_str(), nullptr, nullptr);

        if (!m_Window) {
            throw std::runtime_error("Failed to create window.");
        }
    }
}

void Window::Close() const noexcept {
    if (m_GLFWInitialized) {
        glfwDestroyWindow(m_Window);
        glfwTerminate();
    }
}

}