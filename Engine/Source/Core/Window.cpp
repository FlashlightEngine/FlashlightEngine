#include "Core/Window.hpp"
#include "OpenGLRenderer/GLLoader.hpp"

#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>


Window::Window(const WindowProperties& properties) : m_Properties(properties) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    m_Window = glfwCreateWindow(properties.Width, properties.Height, properties.Title.c_str(), nullptr, nullptr);
    if (m_Window == nullptr) {
        throw std::runtime_error("Failed to create window.");
    }
    glfwMakeContextCurrent(m_Window);

    // Store the initial resolution.
    m_CurrentWidth = properties.Width;
    m_CurrentHeight = properties.Height;

    InitCallbacks();
}

Window::~Window() { glfwDestroyWindow(m_Window); }

WindowProperties Window::GetProperties() const noexcept {
    return m_Properties;
}

bool Window::ShouldClose() const noexcept {
    return glfwWindowShouldClose(m_Window);
}

void Window::Update() const noexcept {
    glfwSwapBuffers(m_Window);
    glfwPollEvents();
}

void Window::Terminate() const noexcept {
    glfwTerminate();
}

void Window::FramebufferSizeCallback(GLFWwindow *window, int width, int height) noexcept {
    GLLoader::UpdateViewportSize(width, height);
}

void Window::InitCallbacks() const noexcept {
    glfwSetWindowSizeCallback(m_Window, FramebufferSizeCallback);
}

void Window::UpdateWindowProperties() noexcept {
    int newWidth = 0, newHeight = 0;
    glfwGetWindowSize(m_Window, &newWidth, &newHeight);

    // Check if the window is resized.
    if (newWidth != m_CurrentWidth || newHeight != m_CurrentHeight) {

        // Set the width and height stored in properties to the new values.
        m_Properties.Width =  newWidth;
        m_Properties.Height = newHeight;

        m_CurrentWidth = newWidth;
        m_CurrentHeight = newHeight;
    }
}
