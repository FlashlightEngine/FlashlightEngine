// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in LICENSE

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
        m_Data.VSyncEnabled = windowProperties.VSync;
        m_Data.Focused = true;

        glfwSetWindowUserPointer(m_Window, &m_Data);

        SetVSync(windowProperties.VSync);

        // ------------------------------- Window callbacks ------------------------------- //
        Log::EngineTrace("Setting up window callbacks...");
        
        glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) {
            const auto data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
            WindowCloseEvent event;
            data->EventCallback(event);
        });

        glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, const i32 width, const i32 height) {
            const auto data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
            data->Width = width;
            data->Height = height;
            data->ShouldInvalidateSwapchain = true;
            WindowResizeEvent event(width, height);
            data->EventCallback(event);
        });

        glfwSetWindowPosCallback(m_Window, [](GLFWwindow* window, const i32 x, const i32 y) {
            const auto data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
            WindowMovedEvent event(x, y);
            data->EventCallback(event);
        });

        glfwSetKeyCallback(m_Window, [](GLFWwindow* window, const i32 key, [[maybe_unused]] const i32 scancode,
                                        const i32 action,
                                        [[maybe_unused]] const i32 mods) {
            const auto data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
            switch (action) {
            case GLFW_PRESS:
                {
                    KeyDownEvent event(key, 0);
                    data->EventCallback(event);
                    break;
                }
            case GLFW_RELEASE:
                {
                    KeyUpEvent event(key);
                    data->EventCallback(event);
                    break;
                }
            case GLFW_REPEAT:
                {
                    KeyDownEvent event(key, 1);
                    data->EventCallback(event);
                    break;
                }
            default:
                Log::EngineError("Unknown GLFW key action.");
            }
        });

        glfwSetCharCallback(m_Window, [](GLFWwindow* window, const u32 keycode) {
            const auto data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
            KeyTypedEvent event(static_cast<i32>(keycode));
            data->EventCallback(event);
        });

        glfwSetMouseButtonCallback(
            m_Window, [](GLFWwindow* window, const i32 button, const i32 action, [[maybe_unused]] const i32 mods) {
                const auto data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));

                switch (action) {
                case GLFW_PRESS:
                    {
                        MouseButtonDownEvent event(button);
                        data->EventCallback(event);
                        break;
                    }
                case GLFW_RELEASE:
                    {
                        MouseButtonUpEvent event(button);
                        data->EventCallback(event);
                        break;
                    }
                default:
                    Log::EngineError("Unknown GLFW mouse button action.");
                }
            });

        glfwSetScrollCallback(m_Window, [](GLFWwindow* window, const f64 xOffset, const f64 yOffset) {
            const auto data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
            MouseScrolledEvent event(static_cast<f32>(xOffset), static_cast<f32>(yOffset));
            data->EventCallback(event);
        });

        glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, const f64 xPos, const f64 yPos) {
            const auto data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
            MouseMovedEvent event(static_cast<f32>(xPos), static_cast<f32>(yPos));
            data->EventCallback(event);
        });

        glfwSetWindowFocusCallback(m_Window, [](GLFWwindow* window, const i32 focused) {
            const auto data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
            WindowFocusedEvent event(focused);
            data->EventCallback(event);
            data->Focused = focused;
        });

        Log::EngineTrace("Callbacks are set up successfully.");
    }

    Window::~Window() {
        if (m_Window != nullptr) {
            ImGui_ImplGlfw_Shutdown();

            Log::EngineTrace("Destroying window.");
            glfwDestroyWindow(m_Window);
        }

        glfwTerminate();
    }

    void Window::Update() {
        glfwPollEvents();
    }

    void Window::SetMouseMovementCallback(const GLFWcursorposfun callback) const {
        glfwSetCursorPosCallback(m_Window, callback);
    }
}
