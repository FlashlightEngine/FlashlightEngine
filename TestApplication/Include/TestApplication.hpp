#pragma once

#include <FlashlightEngine/FlashLightEngine.hpp>

namespace Flashlight {

class TestApplication : public Application {
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

};