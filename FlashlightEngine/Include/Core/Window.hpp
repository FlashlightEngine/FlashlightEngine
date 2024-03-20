#pragma once

#include "pch.hpp"

namespace Flashlight {

struct WindowProperties {
    int32 Width, Height;
    std::string Title;

    explicit WindowProperties(int32 width = 1920, int32 height = 1080,
                              std::string title = "Vulkan Window") : Width(width), Height(height),
                                                                     Title(std::move(title)) {
    }
};

class Window {
public:
    Window(WindowProperties properties);
    ~Window();

    bool ShouldClose() const noexcept;
    WindowProperties GetProperties() const noexcept;

    void Update();

    static std::unique_ptr<Window> Create();
    static std::unique_ptr<Window> Create(int32 Width, int32 Height, std::string Title);

private:
    GLFWwindow *m_Window;

    WindowProperties m_Properties;

    bool m_GLFWInitialized = false;

    void Init();
    void Close() const noexcept;
};

}