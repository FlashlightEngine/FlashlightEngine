/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Window.hpp
 * Description : Declaration of the Window class. The Window class is used to create and manage a GLFW window.
 */
#pragma once

#include "FlashlightEngine/Core/Logger.hpp"

#include "FlashlightEngine/pch.hpp"

#include <GLFW/glfw3.h>

namespace Flashlight {
    struct WindowProperties {
        i32 Width, Height;
        std::string Title;

        WindowProperties(const i32 width, const i32 height, std::string&& title) : Width(width), Height(height),
                                                                                   Title(std::move(title)) {
        }
    };

    struct WindowData {
        i32 Width, Height;
    };

    class Window {
        GLFWwindow* m_Window;
        WindowData m_Data;
        
    public:
        explicit Window(const WindowProperties& windowProperties);
        inline ~Window();

        Window(const Window&) = delete;
        Window(Window&&) = delete;

        Window& operator=(const Window&) = delete;
        Window& operator=(Window&&) = delete;

        [[nodiscard]] inline GLFWwindow* GetGlfwWindow() const;
        [[nodiscard]] inline bool ShouldClose() const;
        [[nodiscard]] inline i32 GetWidth() const;
        [[nodiscard]] inline i32 GetHeight() const;

        static inline void Update();
        inline void Close() const;
    };

#include "Window.inl"
}
