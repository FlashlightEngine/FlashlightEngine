/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Window.cpp
 * Description : Definitions of methods from the Window class.
 */
#include "FlashlightEngine/Core/Window.hpp"

#include "FlashlightEngine/Core/Logger.hpp"

namespace Flashlight {
    Window::Window(const WindowProperties& windowProperties) {
        if (!glfwInit()) {
            Log::EngineFatal({0x02, 0x00}, "Failed to initialize GLFW.");
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        Log::EngineTrace("Creating window.");

        m_Window = glfwCreateWindow(windowProperties.Width, windowProperties.Height, windowProperties.Title.c_str(),
                                    nullptr, nullptr);

        if (!m_Window) {
            Log::EngineFatal({0x02, 0x01}, "Failed to create window.");
        } else {
            Log::EngineTrace("Window created.");
        }

        glfwSetWindowUserPointer(m_Window, &m_Data);

        glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, const int width, const int height) {
            const auto data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
            data->Width = width;
            data->Height = height;
        });
    }
}
