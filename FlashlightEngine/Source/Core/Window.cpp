/* Copyright (C) 2024 Jean "Pixfri" Letessier (jean.letessier@protonmail.com)
 * This file is part of "FlashLight Engine"
 * For conditions of distribution and use, see copyright notice in FlashLightEngine.hpp
 *
 * File : Window.cpp
 * Description This file contains the definitions of methods from the Flashlight::Window class.
 */

#include "FlashlightEngine/Core/Window.hpp"

#include "FlashlightEngine/pch.hpp"

namespace Flashlight {

/// @ingroup Core
/// @class Flashlight::Window
/// @brief Core class that represents the window

/// @brief Initializes GLFW and the native GLFW window.
void Window::Init() {
    if (!m_GLFWInitialized) {
        glfwInit();
        m_GLFWInitialized = true;

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        m_Window = glfwCreateWindow(m_Properties.Width, m_Properties.Height, m_Properties.Title.c_str(), nullptr, nullptr);

        if (!m_Window) {
            std::cerr << "Failed to create window.";
            throw std::runtime_error("");
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