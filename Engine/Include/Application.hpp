#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include "Core/Window.hpp"

#include <memory>

class Application {
public:
    ~Application() = default;

    void Run();

private:
    std::unique_ptr<Window> m_Window;

    void Start();
    void MainLoop();
    void End();
};

#endif // APPLICATION_HPP