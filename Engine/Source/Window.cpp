#include "Window.hpp"

#include <GLFW/glfw3.h>
#include <iostream>

Window::Window(int width, int height) {
  CreateWindow(width, height, "Engine Window");
}

Window::Window(int width, int height, std::string &title) {
  CreateWindow(width, height, title);
}

Window::~Window() { glfwDestroyWindow(m_Window); }

void Window::CreateWindow(int width, int height, std::string title) {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  m_Window = glfwCreateWindow(800, 600, "Engine Window", nullptr, nullptr);
  if (m_Window == nullptr) {
    std::cout << "Failed to create window!";
  }
  glfwMakeContextCurrent(m_Window);
}
