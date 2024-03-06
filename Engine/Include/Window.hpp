#ifndef WINDOW_HPP
#define WINDOW_HPP

// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on

#include <string>

class Window {
public:
  Window(int width, int height);
  Window(int width, int height, std::string &title);
  ~Window();

private:
  GLFWwindow *m_Window;

  void CreateWindow(int width, int height, std::string title);
};

#endif // WINDOW_HPP
