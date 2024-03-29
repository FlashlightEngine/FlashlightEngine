#pragma once

#include <FlashlightEngine/FlashLightEngine.hpp>

namespace TestApplication {

class TestApplication : public Flashlight::Application {
public:
    TestApplication() = default;
    ~TestApplication() override = default;

    // Cannot copy/move an application.
    TestApplication(const TestApplication &application) = delete;
    TestApplication &operator=(const TestApplication &application) = delete;
    TestApplication(const TestApplication &&application) = delete;
    TestApplication &operator=(const TestApplication &&application) = delete;

private:
    void Update() override;
    void Render() override;
};

}