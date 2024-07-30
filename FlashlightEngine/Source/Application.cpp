/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Application.cpp
 * Description : Definitions of methods from the Application class.
 */
#include <FlashlightEngine/Application.hpp>

#include <FlashlightEngine/Core/Logger.hpp>

namespace Flashlight {
    Application* Application::m_SLoadedApplication = nullptr;

    void Application::Run(const WindowProperties& windowProperties, const DebugLevel& debugLevel) {
        assert(!m_SLoadedApplication);
        m_SLoadedApplication = this;

        if (!Init(windowProperties, debugLevel)) {
            Log::AppFatal({0x00, 0x01}, "Failed to initialize application.");
        }

        Log::AppInfo("Launching application.");

        while (m_IsRunning) {
            Update();

            Render();
        }

        Log::AppInfo("Quitting application.");

        Cleanup();
    }

    bool Application::Init(const WindowProperties& windowProperties, const DebugLevel& debugLevel) {
        Logger::Init();

        m_Window = std::make_unique<Window>(windowProperties);
        m_Renderer = std::make_unique<VulkanRenderer::VulkanRenderer>(*m_Window, debugLevel);

        return true;
    }

    void Application::Update() {
        // Compute delta time.
        const auto oldTime = m_CurrentTime;
        m_CurrentTime = std::chrono::high_resolution_clock::now();

        const std::chrono::duration<double, std::milli> timeSpan = (m_CurrentTime - oldTime);
        m_DeltaTime = static_cast<float>(timeSpan.count()) / 1000.0f;

        // Check if window is closed.
        if (m_Window->ShouldClose()) {
            m_IsRunning = false;
        }

        // Poll window events.
        m_Window->Update();

        if (m_Window->ShouldStopRendering()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    void Application::Cleanup() {
        m_SLoadedApplication = nullptr;
    }
}
