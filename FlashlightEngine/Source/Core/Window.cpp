/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Window.cpp
 * Description : Definitions of methods from the Window class.
 */
#include "FlashlightEngine/Core/Window.hpp"

#include "FlashlightEngine/Core/Logger.hpp"

#include <glad/glad.h>

namespace Flashlight {
    Window::Window(const WindowProperties& windowProperties) {
        if (!glfwInit()) {
            Log::EngineFatal({0x01, 0x00}, "Failed to initialize GLFW.");
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#endif

        Log::EngineTrace("Creating window.");

        m_Window = glfwCreateWindow(windowProperties.Width, windowProperties.Height, windowProperties.Title.c_str(),
                                    nullptr, nullptr);

        if (!m_Window) {
            glfwTerminate();
            Log::EngineFatal({0x01, 0x01}, "Failed to create window.");
        } else {
            Log::EngineTrace("Window created.");
        }
        glfwMakeContextCurrent(m_Window);

        Log::EngineTrace("Loading OpenGL...");
        // (clangd says that this cast isn't possible even though it is.)
        if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) { // NOLINT(clang-diagnostic-cast-function-type-strict) 
            Log::EngineFatal({0x01, 0x02}, "Failed to initialize OpenGL.");
        } else {
            Log::EngineTrace("OpenGL loaded.");
        }
        m_Data.Width = windowProperties.Width;
        m_Data.Height = windowProperties.Height;
        glViewport(0, 0, m_Data.Width, windowProperties.Height);

        glfwSetWindowUserPointer(m_Window, &m_Data);

        glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, const int width, const int height) {
            glViewport(0, 0, width, height);
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
