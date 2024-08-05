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
        glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_FALSE);

        const GLFWvidmode* videoMode = glfwGetVideoMode(glfwGetPrimaryMonitor());

        Log::EngineTrace("Creating window...");
        const i32 width = windowProperties.Fullscreen ? videoMode->width : windowProperties.Width;
        const i32 height = windowProperties.Fullscreen ? videoMode->height : windowProperties.Height;

        // If windowProperties.Fullscreen is true, discard the width and height and use the monitor's ones instead. 
        m_Window = glfwCreateWindow(width, height, windowProperties.Title.c_str(),
                                    windowProperties.Fullscreen ? glfwGetPrimaryMonitor() : nullptr, nullptr);

        if (m_Window == nullptr) {
            Log::EngineFatal({0x01, 0x01}, "Failed to create GLFW window.");
        }

        if (!windowProperties.Fullscreen) {
            const i32 windowLeft = videoMode->width / 2 - windowProperties.Width / 2;
            const i32 windowTop = videoMode->height / 2 - windowProperties.Height / 2;
            glfwSetWindowPos(m_Window, windowLeft, windowTop);
        }

        Log::EngineTrace("Window created.");

        m_Data.Width = width;
        m_Data.Height = height;
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
