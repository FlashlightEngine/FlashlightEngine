#include "FlashlightEngine/Core/Window.hpp"

#include "FlashlightEngine/Core/Logger.hpp"

namespace Flashlight {
    void Window::Create(const WindowProperties& windowProperties) {
        if (!glfwInit()) {
            Log::EngineFatal({0x02, 0x00}, "Failed to initialize GLFW.");
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        m_Window = glfwCreateWindow(windowProperties.Width, windowProperties.Height, windowProperties.Title.c_str(),
                                    nullptr, nullptr);

        if (!m_Window) {
            Log::EngineFatal({0x02, 0x01}, "Failed to create window.");
        }
    }

    void Window::Destroy() const {
        if (m_Window) {
            glfwDestroyWindow(m_Window);
        }

        glfwTerminate();
    }
}
