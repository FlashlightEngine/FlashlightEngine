#include <FlashlightEngine/FlashlightEngine.hpp>

int main(int argc, char* argv[]) {
    Flashlight::FlashlightEngine app{};

    const Flashlight::WindowProperties windowProperties{800, 600, "Flashlight Editor"};
    
    app.Run(windowProperties, Flashlight::DebugLevel::Debug);

    return 0;
}
