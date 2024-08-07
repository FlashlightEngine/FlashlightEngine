#include <FlashlightEngine/FlashlightEngine.hpp>

int main(int argc, char* argv[]) {
    Flashlight::FlashlightEngine app{};

    const Flashlight::WindowProperties windowProperties{1920, 1080, "Flashlight Editor", false, false};

#if defined (FL_DEBUG)
    constexpr auto debugLevel = Flashlight::DebugLevel::Debug;
#else
    constexpr auto debugLevel = Flashlight::DebugLevel::None;
#endif

    app.Run(windowProperties, debugLevel);

    return 0;
}
