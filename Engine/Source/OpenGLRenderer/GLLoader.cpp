#include "OpenGLRenderer/GLLoader.hpp"

#include "Core/Window.hpp"

// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on

#include <stdexcept>

void GLLoader::LoadGLLoader(const WindowProperties &windowProperties) {
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        throw std::runtime_error("Failed to initalize GLAD");
    }
    glViewport(0, 0, windowProperties.Width, windowProperties.Height);
}

void GLLoader::EnableGLFeature(const GLenum feature) {
    glEnable(feature);
}

void GLLoader::UpdateViewportSize(int width, int  height) {
    glViewport(0, 0, width, height);
}
