/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Application.cpp
 * Description : Definitions of methods from the Application class.
 */
#include <FlashlightEngine/Application.hpp>

#include <FlashlightEngine/Core/Logger.hpp>

#include <imgui.h>

namespace Flashlight {    
    Application* Application::m_SLoadedApplication = nullptr;

    Application::Application(const WindowProperties& windowProperties, const DebugLevel& debugLevel) {
        assert(!m_SLoadedApplication && "Only one instance of this application can run at a time.");
        m_SLoadedApplication = this;
        
        Logger::Init();

        m_Window = std::make_unique<Window>(windowProperties);
        m_Window->SetEventCallback([this](Event& event) {
            EventDispatcher dispatcher(event);
            dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_TO_EVENT_HANDLER(Application::OnWindowClose));
            OnEvent(event);
        });
        
        m_Renderer = std::make_unique<Renderer::VulkanRenderer>(*m_Window, debugLevel);
        
        m_IsRunning = true;
        
        Log::EditorInfo("Launching application.");
    }

    Application::~Application() {
        Log::EditorInfo("Quitting application.");
        
        m_SLoadedApplication = nullptr;
    }

    void Application::Run() {
        while (m_IsRunning) {
            m_Window->Update();
            
            // Compute delta time.
            const auto oldTime = m_CurrentTime;
            m_CurrentTime = std::chrono::high_resolution_clock::now();

            const std::chrono::duration<double, std::milli> timeSpan = (m_CurrentTime - oldTime);
            m_DeltaTime = static_cast<f32>(timeSpan.count()) / 1000.0f;

            m_EngineStats.FrameTime = m_DeltaTime;
            
            OnUpdate();

            OnRender();
        }
    }

    void Application::OnWindowClose(Event& event) {
        m_IsRunning = false;
    }
}
