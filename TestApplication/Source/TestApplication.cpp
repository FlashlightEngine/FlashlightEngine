#include <TestApplication.hpp>

#include <imgui.h>

TestApplication::TestApplication(const Flashlight::WindowProperties& windowProperties,
                                 const Flashlight::DebugLevel& debugLevel) : Application(
    windowProperties, debugLevel) {
    m_Camera.Velocity = glm::vec3(0.f);
    m_Camera.Position = glm::vec3(30.f, -00.f, -085.f);

    m_Camera.Pitch = 0;
    m_Camera.Yaw = 0;

    m_IsRunning = true;
}

void TestApplication::OnEvent(Flashlight::Event& event) {
    Flashlight::EventDispatcher dispatcher(event);

    dispatcher.Dispatch<Flashlight::KeyDownEvent>(BIND_EVENT_TO_EVENT_HANDLER(TestApplication::OnKeyDown));
    dispatcher.Dispatch<Flashlight::KeyUpEvent>(BIND_EVENT_TO_EVENT_HANDLER(TestApplication::OnKeyUp));
    dispatcher.Dispatch<Flashlight::MouseMovedEvent>(BIND_EVENT_TO_EVENT_HANDLER(TestApplication::OnMouseMovement));
}

void TestApplication::OnUpdate() {
}

void TestApplication::OnRender() {
    m_Renderer->CreateRendererUi();

    CreateEditorUi();

    m_Renderer->Draw(*m_Window, m_Camera, m_EngineStats);
}

void TestApplication::OnKeyDown(const Flashlight::KeyDownEvent& event) {
    if (event.GetScancode() == static_cast<i32>(Flashlight::Keys::Escape)) {
        m_IsRunning = false;
    }

    m_Camera.OnKeyDown(event.GetScancode(), m_DeltaTime, m_CameraSpeed);
}

void TestApplication::OnKeyUp(const Flashlight::KeyUpEvent& event) {
    m_Camera.OnKeyUp(event.GetScancode());
}

void TestApplication::OnMouseMovement(const Flashlight::MouseMovedEvent& event) {
    m_Camera.OnMouseMovement(event.GetX(), event.GetY(), m_MouseSensitivity * 5.f, m_DeltaTime);
}

void TestApplication::CreateEditorUi() {
    if (ImGui::Begin("Camera")) {
        ImGui::SliderFloat("Camera speed", &m_CameraSpeed, 0.1f, 10.0f);
        ImGui::SliderFloat("Mouse sensitivity", &m_MouseSensitivity, 0.1f, 10.0f);
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
