/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Window.cpp
 * Description : Definitions of methods from the Window class.
 */
#include "FlashlightEngine/Core/Window.hpp"

#include "FlashlightEngine/Core/Logger.hpp"

#include "FlashlightEngine/pch.hpp"

namespace Flashlight {
    void Window::Create(const WindowProperties& windowProperties) {
        if (!glfwInit()) {
            Log::EngineFatal({0x02, 0x00}, "Failed to initialize GLFW.");
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        Log::EngineTrace("Creating window.");

        m_Window = glfwCreateWindow(windowProperties.Width, windowProperties.Height, windowProperties.Title.c_str(),
                                    nullptr, nullptr);

        if (!m_Window) {
            Log::EngineFatal({0x02, 0x01}, "Failed to create window.");
        }

        Log::EngineTrace("Window created.");
    }

    void Window::Destroy() const {
        if (m_Window) {
            Log::EngineTrace("Destroying window.");
            glfwDestroyWindow(m_Window);
        }

        glfwTerminate();
    }
}
