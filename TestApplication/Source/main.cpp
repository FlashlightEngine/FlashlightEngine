#include "TestApplication.hpp"

int main() {
    TestApplication app{};

    const Flashlight::WindowProperties windowProperties {640, 480, "Test Application"};

    app.Run(windowProperties);

    return 0;
}
