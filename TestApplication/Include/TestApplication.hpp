#pragma once

#include "FlashlightEngine/Application.hpp"

class TestApplication final : public Flashlight::Application {
    std::unique_ptr<Flashlight::GraphicsPipeline> m_GraphicsPipeline;
    
public:
    ~TestApplication() override = default;

    bool Init() override;
    void Update() override;
    void Render() override;
    void Cleanup() override;
};
