#include "TestApplication.hpp"

#include <FlashlightEngine/EntryPoint.hpp>

std::unique_ptr<Flashlight::Application> Flashlight::CreateApp() {
    return std::make_unique<TestApplication>();
}
