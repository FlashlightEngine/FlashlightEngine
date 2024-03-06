#ifndef CORE_WINDOW_HPP
#define CORE_WINDOW_HPP

// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on

#include <string>
#include <tuple>
#include <utility>

struct WindowProperties {
    WindowProperties(const int windowWidth, const int windowHeight, std::string windowTitle)
        : Width(windowWidth), Height(windowHeight), Title(std::move(windowTitle)) {}
    int Width, Height;
    std::string Title;
};

class Window {
public:
    explicit Window(const WindowProperties& properties);
    ~Window();

    WindowProperties GetProperties() const noexcept;
    [[nodiscard]] bool ShouldClose() const noexcept;

    void Update() const noexcept;
    void Terminate() const noexcept;

    static void FramebufferSizeCallback(GLFWwindow *window, int width, int height) noexcept;

private:
    GLFWwindow *m_Window;
    WindowProperties m_Properties;
    int m_CurrentWidth, m_CurrentHeight;

    void InitCallbacks() const noexcept;

    void UpdateWindowProperties() noexcept;
};

#endif // CORE_WINDOW_HPP
