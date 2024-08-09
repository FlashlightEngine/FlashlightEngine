#include <FlashlightEditor/FlashlightEditorApplication.hpp>

int main() {
    Flashlight::WindowProperties properties{1920, 1080, "Test Application", false, false};

#ifdef NDEBUG // NDEBUG = not debug
    constexpr auto debugLevel = Flashlight::DebugLevel::None;
#else
    constexpr auto debugLevel = Flashlight::DebugLevel::Debug;
#endif
        
    const auto app = std::make_unique<FlashlightEditor::FlashlightEditorApplication>(properties, debugLevel);

    app->Run();

    return EXIT_SUCCESS;
}