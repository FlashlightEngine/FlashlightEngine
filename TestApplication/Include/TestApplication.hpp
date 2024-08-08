#pragma once

#include <FlashlightEngine/Application.hpp>

class TestApplication final : public Flashlight::Application {
public:
    TestApplication(const Flashlight::WindowProperties& windowProperties, const Flashlight::DebugLevel& debugLevel);
    ~TestApplication() override = default;

    TestApplication(const TestApplication&) = delete;
    TestApplication(TestApplication&&) = delete;

    TestApplication& operator=(const TestApplication&) = delete;
    TestApplication& operator=(TestApplication&&) = delete;

protected:
    void OnEvent(Flashlight::Event& event) override;
    void OnUpdate() override;
    void OnRender() override;
    
    Flashlight::Renderer::Camera m_Camera;
    f32 m_CameraSpeed = 1.f;
    f32 m_MouseSensitivity = 1.f;

    void OnKeyDown(const Flashlight::KeyDownEvent& event);
    void OnKeyUp(const Flashlight::KeyUpEvent& event);
    void OnMouseMovement(const Flashlight::MouseMovedEvent& event);

    void CreateEditorUi();
};