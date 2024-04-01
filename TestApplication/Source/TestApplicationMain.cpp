#include "TestApplication.hpp"

#include <FlashlightEngine/FlashlightEngine.hpp>

int main() {
    Flashlight::Logger::Init();
    
    std::unique_ptr<TestApplication::TestApplication> application = std::make_unique<TestApplication::TestApplication>();

    try {
        Flashlight::Log::Info("Starting application.");
        application->Run();
    } catch (const std::exception &e) {
        Flashlight::Log::Error(e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
