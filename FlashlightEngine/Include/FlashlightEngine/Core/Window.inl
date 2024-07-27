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

inline VkExtent2D Window::GetWindowExtent() const {
    return {static_cast<u32>(m_Data.Width), static_cast<u32>(m_Data.Height)};
}


inline bool Window::ShouldRecreateSwapChain() const {
    return m_Data.ShouldRecreateSwapChain;
}

inline void Window::SwapChainRecreated() {
    m_Data.ShouldRecreateSwapChain = false;
}

inline void Window::Update() {
    glfwPollEvents();
}

inline void Window::Close() const {
    glfwSetWindowShouldClose(m_Window, true);
}
