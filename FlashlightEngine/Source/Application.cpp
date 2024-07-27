/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Application.cpp
 * Description : Definitions of methods from the Application class.
 */
#include "FlashlightEngine/Application.hpp"

#include "FlashlightEngine/Core/Logger.hpp"

namespace Flashlight {
    void Application::Run(const WindowProperties &windowProperties) {
        Log::AppInfo("Launching application.");

        if (!Init(windowProperties)) {
            Log::AppFatal({0x00, 0x01}, "Failed to initialize application.");
        }

        while (m_IsRunning) {
            Update();

            Render();
        }

        Log::AppInfo("Quitting application.");
        
        Cleanup();
    }

    bool Application::Init(const WindowProperties& windowProperties) {
        Logger::Init();

        m_Window = std::make_unique<Window>(windowProperties);

        return true;
    }

    void Application::Update() {
        if (m_Window->ShouldClose()) {
            m_IsRunning = false;
        }

        m_Window->Update();
    }
}
