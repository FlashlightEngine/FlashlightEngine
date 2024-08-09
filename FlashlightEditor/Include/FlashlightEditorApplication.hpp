#pragma once

#include <FlashlightEngine/Application.hpp>
#include <FlashlightEngine/Managers/SceneManager.hpp>

class FlashlightEditorApplication final : public Flashlight::Application {
    std::unique_ptr<Flashlight::SceneManager> m_SceneManager;
    
public:
    FlashlightEditorApplication(const Flashlight::WindowProperties& windowProperties, const Flashlight::DebugLevel& debugLevel);
    ~FlashlightEditorApplication() override = default;

    FlashlightEditorApplication(const FlashlightEditorApplication&) = delete;
    FlashlightEditorApplication(FlashlightEditorApplication&&) = delete;

    FlashlightEditorApplication& operator=(const FlashlightEditorApplication&) = delete;
    FlashlightEditorApplication& operator=(FlashlightEditorApplication&&) = delete;

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