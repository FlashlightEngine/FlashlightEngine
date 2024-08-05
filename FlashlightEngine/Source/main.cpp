#include <FlashlightEngine/FlashlightEngine.hpp>

int main(int argc, char* argv[]) {
    Flashlight::FlashlightEngine app{};

    const Flashlight::WindowProperties windowProperties{1920, 1080, "Flashlight Editor", false, false};
    
    app.Run(windowProperties, Flashlight::DebugLevel::Debug);

    return 0;
}
