#include "Application.hpp"

#include "pch.hpp"

int main() {
    Flashlight::Application app;

    try {
        app.Run();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}