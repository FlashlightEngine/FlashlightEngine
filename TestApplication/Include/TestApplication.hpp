#pragma once

#include <FlashlightEngine/Application.hpp>

class TestApplication final : public Flashlight::Application {
    public:
    TestApplication() = default;
    ~TestApplication() override = default;

    TestApplication(const TestApplication&) = delete;
    TestApplication(TestApplication&&) = delete;

    TestApplication& operator=(const TestApplication&) = delete;
    TestApplication& operator=(TestApplication&&) = delete;

protected:
    bool Init(const Flashlight::WindowProperties& windowProperties, const Flashlight::DebugLevel& debugLevel) override;
    void Update() override;
    void Render() override;
    void Cleanup() override;
};
