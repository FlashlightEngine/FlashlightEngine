#include "TestApplication.hpp"

#undef main

int main(int argc, char* argv[]) {
    TestApplication app{};

    const Flashlight::WindowProperties windowProperties {800, 600, "Test Application" };
    
    app.Run(windowProperties, DebugLevel::Debug);

    return 0;
}
