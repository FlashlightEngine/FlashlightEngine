#include <TestApplication.hpp>

#include <FlashlightEngine/VulkanRenderer/VulkanUtils/VulkanBufferUtils.hpp>
#include <FlashlightEngine/VulkanRenderer/VulkanInitializers.hpp>

#include <imgui.h>

TestApplication::TestApplication(const Flashlight::WindowProperties& windowProperties,
                                 const Flashlight::DebugLevel& debugLevel) : Application(
    windowProperties, debugLevel) {
    m_Camera.Velocity = glm::vec3(0.f);
    m_Camera.Position = glm::vec3(30.f, -00.f, -085.f);

    m_Camera.Pitch = 0;
    m_Camera.Yaw = 0;

    m_SceneManager = std::make_unique<Flashlight::SceneManager>(m_Renderer.get());

    m_SceneManager->LoadScene("Resources/Models/structure.glb", "structure");

    m_IsRunning = true;
}

void TestApplication::OnEvent(Flashlight::Event& event) {
    Flashlight::EventDispatcher dispatcher(event);

    dispatcher.Dispatch<Flashlight::KeyDownEvent>(BIND_EVENT_TO_EVENT_HANDLER(TestApplication::OnKeyDown));
    dispatcher.Dispatch<Flashlight::KeyUpEvent>(BIND_EVENT_TO_EVENT_HANDLER(TestApplication::OnKeyUp));
    dispatcher.Dispatch<Flashlight::MouseMovedEvent>(BIND_EVENT_TO_EVENT_HANDLER(TestApplication::OnMouseMovement));
}

void TestApplication::OnUpdate() {
    const auto start = std::chrono::system_clock::now();
    m_Camera.Update();

    m_SceneManager->UpdateScene(m_Camera, *m_Window, "structure");

    const auto end = std::chrono::system_clock::now();

    const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    m_EngineStats.SceneUpdateTime = static_cast<f32>(elapsed.count()) / 1000.f;
}

void TestApplication::OnRender() {
    m_Renderer->BeginUi();

    CreateEditorUi();

    m_Renderer->BeginRendering(*m_Window, {{0.1f, 0.1f, 0.1f, 1.0f}}, m_EngineStats);

    m_SceneManager->RenderScene("structure", m_EngineStats);
    
    m_Renderer->EndRendering(*m_Window);
}

void TestApplication::OnKeyDown(const Flashlight::KeyDownEvent& event) {
    if (event.GetScancode() == static_cast<i32>(Flashlight::Keys::Escape)) {
        m_IsRunning = false;
    }

    m_Camera.OnKeyDown(event.GetScancode(), m_DeltaTime, m_CameraSpeed * 2.25f);
}

void TestApplication::OnKeyUp(const Flashlight::KeyUpEvent& event) {
    m_Camera.OnKeyUp(event.GetScancode());
}

void TestApplication::OnMouseMovement(const Flashlight::MouseMovedEvent& event) {
    m_Camera.OnMouseMovement(event.GetX(), event.GetY(), m_MouseSensitivity * 2.5f, m_DeltaTime);
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
