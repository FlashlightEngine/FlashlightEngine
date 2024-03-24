/* Copyright (C) 2024 Jean "Pixfri" Letessier (jean.letessier@protonmail.com)
 * This file is part of "FlashLight Engine"
 * For conditions of distribution and use, see copyright notice in FlashLightEngine.hpp
 *
 * Window.cpp - Definitions of the Window class methods.
 * This file contains the definitions of methods from the Flashlight::Window class.
 */

#include "FlashlightEngine/Core/Window.hpp"

#include "FlashlightEngine/pch.hpp"

namespace Flashlight {

/// @ingroup Core
/// @class Flashlight::Window
/// @brief Core class that represents the window

/// @brief The window construction
///
/// @param properties The window properties
Window::Window(WindowProperties properties) : m_Properties(std::move(properties)) {
    Init();
}

/// @brief The window destruction
Window::~Window() {
    Close();
}

/// @brief Returns if the window should be closed.
///
/// @returns A boolean telling of the window should close.
bool Window::ShouldClose() const noexcept {
    return glfwWindowShouldClose(m_Window);
}

/// @brief Returns the window properties (width, height, titile).
///
/// @returns A WindowProperties struct containing the current properties of the window.
WindowProperties Window::GetProperties() const noexcept {
    return m_Properties;
}

/// @brief Returns the native GLFW window handle.
///
/// @returns A GLFWwindow* to the native GLFW window.
GLFWwindow* Window::GetNativeWindow() const noexcept {
    return m_Window;
}

/// @brief Update the window and poll the events.
void Window::Update() {
    if (m_GLFWInitialized) {
        glfwPollEvents();
    }
}

/// @brief Creates a window with the default values of the WindowProperties constructor.
///
/// @see Flashlight::WindowProperties
///
/// @returns A std::unique_pointer to the newly created window.
std::unique_ptr<Window> Window::Create() {
    return std::make_unique<Window>(WindowProperties()); // Create a window using the default properties.
}

/// @brief Creates a window with specific properties.
///
/// @param properties The window properties struct with the specific window parameters set.
///
/// @returns A std::unique_pointer to the newly created window.
std::unique_ptr<Window> Window::Create(const WindowProperties &properties) {
    return std::make_unique<Window>(properties);
}

/// @brief Initializes GLFW and the native GLFW window.
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

/// @brief Destroys the window then terminates GLFW.
void Window::Close() const noexcept {
    if (m_GLFWInitialized) {
        glfwDestroyWindow(m_Window);
        glfwTerminate();
    }
}

}