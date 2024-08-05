/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Window.cpp
 * Description : Definitions of methods from the Window class.
 */
#include <FlashlightEngine/Core/Window.hpp>

#include <FlashlightEngine/Core/Logger.hpp>

#include <GLFW/glfw3.h>

#include <imgui_impl_glfw.h>

namespace Flashlight {
    Window::Window(const WindowProperties& windowProperties) {
        if (!glfwInit()) {
            Log::EngineFatal({0x01, 0x00}, "Failed to initialize GLFW.");
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        Log::EngineTrace("Creating window...");
        m_Window = glfwCreateWindow(windowProperties.Width, windowProperties.Height, windowProperties.Title.c_str(),
                                    nullptr, nullptr);

        if (m_Window == nullptr) {
            Log::EngineFatal({0x01, 0x01}, "Failed to create GLFW window.");
        }

        Log::EngineTrace("Window created.");

        m_Data.Width = windowProperties.Width;
        m_Data.Height = windowProperties.Height;
        m_Data.Title = windowProperties.Title;

        glfwSetWindowUserPointer(m_Window, &m_Data);

        glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, const i32 width, const i32 height) {
            const auto data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
            data->Width = width;
            data->Height = height;
            data->ShouldInvalidateSwapchain = true;
        });
    }

    Window::~Window() {
        if (m_Window != nullptr) {
            ImGui_ImplGlfw_Shutdown();
            
            Log::EngineTrace("Destroying window.");
            glfwDestroyWindow(m_Window);
        }

        glfwTerminate();
    }

    KeyState Window::GetKeyState(const Keys& key) const {
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

    void Window::Update() {
        glfwPollEvents();

        if (glfwWindowShouldClose(m_Window)) {
            m_Data.ShouldClose = true;
        }
    }

    void Window::SetMouseMovementCallback(const GLFWcursorposfun callback) const {
        glfwSetCursorPosCallback(m_Window, callback);
    }
}
