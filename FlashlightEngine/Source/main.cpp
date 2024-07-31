#include <FlashlightEngine/FlashlightEngine.hpp>

int main(int argc, char* argv[]) {
    Flashlight::FlashlightEngine app{};

    const Flashlight::WindowProperties windowProperties {800, 600, "Test Application" };
    
    app.Run(windowProperties, Flashlight::DebugLevel::Debug);

    return 0;
}
