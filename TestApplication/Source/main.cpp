#include "TestApplication.hpp"

int main() {
    TestApplication app{};

    const Flashlight::WindowProperties windowProperties {800, 600, "Test Application" };
    
    app.Run(windowProperties, Flashlight::DebugLevel::Debug);

    return 0;
}
