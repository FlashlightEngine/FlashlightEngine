/* Copyright (C) 2024 Jean "Pixfri" Letessier (jean.letessier@protonmail.com)
 * This file is part of "FlashLight Engine"
 * For conditions of distribution and use, see copyright notice in FlashLightEngine.hpp
 *
 * Window.inl - Definitions of inline methods from the Window class.
 * This file contains the definitions of inline methods from the Flashlight::Window class.
 */

#pragma once

/// @ingroup Core
/// @class Flashlight::Window
/// @brief Core class that represents the window

/// @brief The window construction
///
/// @param properties The window properties
inline Window::Window(WindowProperties properties) : m_Properties(std::move(properties)) {
    Init();
}

/// @brief The window destruction
inline Window::~Window() {
    Close();
}

/// @brief Returns if the window should be closed.
///
/// @returns A boolean telling of the window should close.
inline bool Window::ShouldClose() const noexcept {
    return glfwWindowShouldClose(m_Window);
}

/// @brief Returns the window properties (width, height, titile).
///
/// @returns A WindowProperties struct containing the current properties of the window.
inline WindowProperties Window::GetProperties() const noexcept {
    return m_Properties;
}

/// @brief Returns the native GLFW window handle.
///
/// @returns A GLFWwindow* to the native GLFW window.
inline GLFWwindow* Window::GetNativeWindow() const noexcept {
    return m_Window;
}

/// @brief Update the window and poll the events.
inline void Window::Update() const noexcept {
    if (m_GLFWInitialized) {
        glfwPollEvents();
    }
}

/// @brief Creates a window with the default values of the WindowProperties constructor.
///
/// @see Flashlight::WindowProperties
///
/// @returns A std::unique_pointer to the newly created window.
inline std::unique_ptr<Window> Window::Create() {
    return std::make_unique<Window>(WindowProperties()); // Create a window using the default properties.
}

/// @brief Creates a window with specific properties.
///
/// @param properties The window properties struct with the specific window parameters set.
///
/// @returns A std::unique_pointer to the newly created window.
inline std::unique_ptr<Window> Window::Create(const WindowProperties &properties) {
    return std::make_unique<Window>(properties);
}