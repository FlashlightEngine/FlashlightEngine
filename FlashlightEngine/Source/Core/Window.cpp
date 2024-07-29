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
            Log::EngineFatal({0x01, 0x00}, "Failed to initialize GLFW.");
        }

        Log::EngineTrace("Creating window.");

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        m_Window = glfwCreateWindow(windowProperties.Width, windowProperties.Height, windowProperties.Title.c_str(),
                                    nullptr, nullptr);

        if (!m_Window) {
            glfwTerminate();
            Log::EngineFatal({0x01, 0x01}, "Failed to create window.");
        } else {
            Log::EngineTrace("Window created.");
        }
        
        m_Data.Width = windowProperties.Width;
        m_Data.Height = windowProperties.Height;

        glfwSetWindowUserPointer(m_Window, &m_Data);

        glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, const int width, const int height) {
            const auto data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
            data->Width = width;
            data->Height = height;
        });
    }

    KeyState Window::GetKeyState(Keys key) const {
        switch (glfwGetKey(m_Window, static_cast<i32>(key))) {
        case GLFW_PRESS:
            return KeyState::Pressed;

        case GLFW_RELEASE:
            return KeyState::Released;

        case GLFW_REPEAT:
            return KeyState::Repeated;

        default:
            return KeyState::None;
        }
    }

}
