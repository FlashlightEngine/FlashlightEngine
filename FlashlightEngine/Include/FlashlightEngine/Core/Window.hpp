/* Copyright (C) 2024 Jean "Pixfri" Letessier (jean.letessier@protonmail.com)
 * This file is part of "FlashLight Engine"
 * For conditions of distribution and use, see copyright notice in FlashLightEngine.hpp
 *
 * Window.hpp - Window class and WindowProperties struct definition.
 * This header file contains the definition of the Window class and the WindowProperties struct.
 */

#pragma once

#include "FlashlightEngine/pch.hpp"

#include <GLFW/glfw3.h>

namespace Flashlight {

/// @ingroup Core
/// @struct Flashlight::WindowProperties
/// @brief Structure definition for window's basic properties
struct WindowProperties {
    int32 Width, Height;
    std::string Title;

    /// @brief The window properties constructor.
    ///
    /// @param width The window's width. By default, 1920 pixels.
    /// @param height The window's height. By default, 1080 pixels.
    /// @param title The window's title. By default, "Flashlight Window"
    explicit WindowProperties(int32 width = 1920, int32 height = 1080,
                              std::string title = "Flashlight Window") : Width(width), Height(height),
                                                                     Title(std::move(title)) {
    }
};

class Window {
public:
    explicit Window(WindowProperties properties);
    ~Window();

    bool ShouldClose() const noexcept;
    WindowProperties GetProperties() const noexcept;

    void Update();

    static std::unique_ptr<Window> Create();
    static std::unique_ptr<Window> Create(const WindowProperties &properties);

private:
    GLFWwindow *m_Window;

    WindowProperties m_Properties;

    bool m_GLFWInitialized = false;

    void Init();
    void Close() const noexcept;
};

}