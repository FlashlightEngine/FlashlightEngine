/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Window.inl
 * Description : Definitions of inline methods from the Window class.
 */
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
