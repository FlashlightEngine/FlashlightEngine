#include "Application.hpp"

// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on

#include <cstdlib>
#include <iostream>

int main() {
    Application application{};

    try {
        application.Run();
    } catch (std::exception &exception) {
        std::cerr << exception.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
