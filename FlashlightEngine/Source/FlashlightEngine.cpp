/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Application.cpp
 * Description : Definitions of methods from the Application class.
 */
#include <FlashlightEngine/FlashlightEngine.hpp>

#include <FlashlightEngine/Core/Logger.hpp>

#include <imgui.h>

namespace Flashlight {    
    FlashlightEngine* FlashlightEngine::m_SLoadedApplication = nullptr;

    void FlashlightEngine::Run(const WindowProperties& windowProperties, const DebugLevel& debugLevel) {
        assert(!m_SLoadedApplication);
        m_SLoadedApplication = this;

        if (!Init(windowProperties, debugLevel)) {
            Log::EditorFatal({0x00, 0x01}, "Failed to initialize application.");
        }

        Log::EditorInfo("Launching application.");

        while (m_IsRunning) {
            Update();

            if (m_Window->ShouldStopRendering()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                continue;
            }

            Render();
        }

        Log::EditorInfo("Quitting application.");

        Cleanup();
    }

    bool FlashlightEngine::Init(const WindowProperties& windowProperties, const DebugLevel& debugLevel) {
        Logger::Init();

        m_Window = std::make_unique<Window>(windowProperties);
        m_Renderer = std::make_unique<Renderer::VulkanRenderer>(*m_Window, debugLevel);

        m_Camera.Velocity = glm::vec3(0.f);
        m_Camera.Position = glm::vec3(30.f, -00.f, -085.f);

        m_Camera.Pitch = 0;
        m_Camera.Yaw = 0;
        
        m_IsRunning = true;
        
        return true;
    }

    void FlashlightEngine::Update() {
        // Compute delta time.
        const auto oldTime = m_CurrentTime;
        m_CurrentTime = std::chrono::high_resolution_clock::now();

        const std::chrono::duration<double, std::milli> timeSpan = (m_CurrentTime - oldTime);
        m_DeltaTime = static_cast<f32>(timeSpan.count()) / 1000.0f;

        m_EngineStats.FrameTime = m_DeltaTime;

        // Check if window is closed.
        if (m_Window->ShouldClose()) {
            m_IsRunning = false;
        }
        
        m_Window->Update();

        m_Camera.ProcessInput(*m_Window, m_DeltaTime, m_CameraSpeed);
    }

    void FlashlightEngine::Render() {
        m_Renderer->CreateRendererUi();

        CreateEditorUi();
        
        m_Renderer->Draw(*m_Window, m_Camera, m_EngineStats);
    }

    void FlashlightEngine::Cleanup() {
        m_SLoadedApplication = nullptr;
    }

    void FlashlightEngine::CreateEditorUi() {
        if (ImGui::Begin("Camera")) {
            ImGui::SliderFloat("Camera speed", &m_CameraSpeed, 0.1f, 10.0f);
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
