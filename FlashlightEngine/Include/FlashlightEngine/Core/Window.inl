/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Window.inl
 * Description : Definitions of inline methods from the Window class.
 */
#pragma once

inline Window::~Window() {
    if (m_Window) {
        Log::EngineTrace("Destroying window.");
        glfwDestroyWindow(m_Window);
    }

    glfwTerminate();
}

inline GLFWwindow* Window::GetGlfwWindow() const {
    return m_Window;
}

inline bool Window::ShouldClose() const {
    return glfwWindowShouldClose(m_Window);
}

inline i32 Window::GetWidth() const {
    return m_Data.Width;
}

inline i32 Window::GetHeight() const {
    return m_Data.Height;
}

inline void Window::Update() const {
    glfwSwapBuffers(m_Window);
    glfwPollEvents();
}

inline void Window::Close() const {
    glfwSetWindowShouldClose(m_Window, true);
}
