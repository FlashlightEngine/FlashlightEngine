// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in LICENSE

#pragma once

inline bool Window::ShouldClose() const {
    return glfwWindowShouldClose(m_Window);
}

inline bool Window::ShouldInvalidateSwapchain() const {
    return m_Data.ShouldInvalidateSwapchain;
}

inline void Window::SwapchainInvalidated() {
    m_Data.ShouldInvalidateSwapchain = false;
}

inline GLFWwindow* Window::GetNativeWindow() const {
    return m_Window;
}

inline i32 Window::GetWidth() const {
    return m_Data.Width;
}

inline i32 Window::GetHeight() const {
    return m_Data.Height;
}

inline VkExtent2D Window::GetExtent() const {
    return { static_cast<u32>(m_Data.Width), static_cast<u32>(m_Data.Height) };
}

inline std::string Window::GetTitle() const {
    return m_Data.Title;
}

inline bool Window::VSyncEnabled() const {
    return m_Data.VSyncEnabled;
}

inline void Window::SetVSync(const bool status) {
    glfwSwapInterval(status);
    m_Data.VSyncEnabled = status;
    m_Data.ShouldInvalidateSwapchain = true;
}

inline void Window::SetEventCallback(const std::function<void(Event&)>& callback) {
    m_Data.EventCallback = callback;
}

inline void Window::Close() const {
    glfwSetWindowShouldClose(m_Window, true);
}
