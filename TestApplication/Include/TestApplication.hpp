#pragma once

#include "FlashlightEngine/Application.hpp"

class TestApplication final : public Flashlight::Application {
public:
    ~TestApplication() override = default;

    bool Init() override;
    void Update() override;
    void Render() override;
    void Cleanup() override;
};
