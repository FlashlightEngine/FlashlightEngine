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

inline WGPUWrapper::SurfaceConfiguration Window::GetDefaultSurfaceConfiguration() const {
    i32 width = 0;
    i32 height = 0;
    glfwGetFramebufferSize(m_Window, &width, &height);

    WGPUWrapper::SurfaceConfiguration surfaceConfiguration = {};
    surfaceConfiguration.Width = static_cast<u32>(width);
    surfaceConfiguration.Height = static_cast<u32>(height);

    return surfaceConfiguration;
}


inline void Window::Update() {
    glfwPollEvents();
}

inline void Window::Close() const {
    glfwSetWindowShouldClose(m_Window, true);
}
