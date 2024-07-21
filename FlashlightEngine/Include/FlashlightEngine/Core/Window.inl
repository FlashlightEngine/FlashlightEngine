#pragma once

inline Window::Window(const WindowProperties &windowProperties) {
    Create(windowProperties);
}

inline Window::~Window() {
    Destroy();
}

inline GLFWwindow* Window::GetGlfwWindow() const {
    return m_Window;
}

inline bool Window::ShouldClose() const {
    return glfwWindowShouldClose(m_Window);
}

inline void Window::Update() {
    glfwPollEvents();
}

inline void Window::Close() const {
    glfwSetWindowShouldClose(m_Window, true);
}
