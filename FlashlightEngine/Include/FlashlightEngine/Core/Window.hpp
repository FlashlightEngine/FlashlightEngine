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

    class Window {
        GLFWwindow* m_Window;

    public:
        explicit inline Window(const WindowProperties& windowProperties);
        inline ~Window();

        Window(const Window&) = delete;
        Window(Window&&) = delete;

        Window& operator=(const Window&) = delete;
        Window& operator=(Window&&) = delete;

        [[nodiscard]] inline GLFWwindow* GetGlfwWindow() const;
        [[nodiscard]] inline bool ShouldClose() const;

        static inline void Update();
        inline void Close() const;

    private:
        void Create(const WindowProperties& windowProperties);
        void Destroy() const;
    };

#include "Window.inl"
}
