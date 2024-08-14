#include <FlashlightEditor/FlashlightEditorApplication.hpp>

#include <imgui.h>

namespace FlashlightEditor {
    FlashlightEditorApplication::FlashlightEditorApplication(const Flashlight::WindowProperties& windowProperties,
                                                             const Flashlight::DebugLevel&
                                                             debugLevel) : Application(
        windowProperties, debugLevel) {
        m_Camera.Velocity = glm::vec3(0.f);
        m_Camera.Position = glm::vec3(0.f, 0.f, 0.f);

        m_Camera.Pitch = 0;
        m_Camera.Yaw = 0;

        m_SceneManager = std::make_unique<Flashlight::SceneManager>(m_Renderer.get());

        m_SceneManager->LoadScene("Resources/Models/DamagedHelmet.glb", "helmet");

        m_IsRunning = true;
    }

    void FlashlightEditorApplication::OnEvent(Flashlight::Event& event) {
        Flashlight::EventDispatcher dispatcher(event);

        dispatcher.Dispatch<Flashlight::KeyDownEvent>(
            BIND_EVENT_TO_EVENT_HANDLER(FlashlightEditorApplication::OnKeyDown));
        dispatcher.Dispatch<Flashlight::KeyUpEvent>(
            BIND_EVENT_TO_EVENT_HANDLER(FlashlightEditorApplication::OnKeyUp));
        dispatcher.Dispatch<Flashlight::MouseMovedEvent>(
            BIND_EVENT_TO_EVENT_HANDLER(FlashlightEditorApplication::OnMouseMovement));
    }

    void FlashlightEditorApplication::OnUpdate() {
        const auto start = std::chrono::system_clock::now();
        m_Camera.Update();

        m_SceneManager->UpdateScene(m_Camera.GetViewMatrix(), glm::radians(m_Fov), *m_Window, "helmet");

        const auto end = std::chrono::system_clock::now();

        const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        m_EngineStats.SceneUpdateTime = static_cast<f32>(elapsed.count()) / 1000.f;
    }

    void FlashlightEditorApplication::OnRender() {
        m_Renderer->BeginUi();

        CreateEditorUi();

        m_Renderer->BeginRendering(*m_Window, {{0.1f, 0.1f, 0.1f, 1.0f}}, m_EngineStats);

        m_SceneManager->RenderScene("helmet", m_EngineStats);

        m_Renderer->EndRendering(*m_Window);
    }

    void FlashlightEditorApplication::OnKeyDown(const Flashlight::KeyDownEvent& event) {
        if (event.GetScancode() == static_cast<i32>(Flashlight::Keys::Escape)) {
            m_IsRunning = false;
        }

        m_Camera.OnKeyDown(event.GetScancode(), m_DeltaTime, m_CameraSpeed * 2.25f);
    }

    void FlashlightEditorApplication::OnKeyUp(const Flashlight::KeyUpEvent& event) {
        m_Camera.OnKeyUp(event.GetScancode());
    }

    void FlashlightEditorApplication::OnMouseMovement(const Flashlight::MouseMovedEvent& event) {
        m_Camera.OnMouseMovement(event.GetX(), event.GetY(), m_MouseSensitivity * 2.5f, m_DeltaTime);
    }

    void FlashlightEditorApplication::CreateEditorUi() {
        if (ImGui::Begin("Camera")) {
            ImGui::SliderFloat("Camera speed", &m_CameraSpeed, 0.1f, 10.0f);
            ImGui::SliderFloat("Mouse sensitivity", &m_MouseSensitivity, 0.1f, 10.0f);
            ImGui::SliderFloat("Camera FOV", &m_Fov, 45.f, 90.f);
        }
        ImGui::End();

        if (ImGui::Begin("Statistics")) {
            ImGui::Text("Frame time %f ms", m_EngineStats.FrameTime);
            ImGui::Text("Draw time %f ms", m_EngineStats.MeshDrawTime);
            ImGui::Text("Update time %f ms", m_EngineStats.SceneUpdateTime);
            ImGui::Text("Triangle count %i", m_EngineStats.TriangleCount);
            ImGui::Text("Draw call count %i", m_EngineStats.DrawCallCount);
            ImGui::Text("Estimated FPS %f", 1.f / m_EngineStats.FrameTime);
        }
        ImGui::End();
    }
}
