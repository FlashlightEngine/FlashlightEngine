#include "FlashlightEngine/Application.hpp"

#include "FlashlightEngine/Core/Logger.hpp"

namespace Flashlight {
    void Application::Run() {
        Logger::Init();

        Log::AppInfo("Launching application.");

        if (!Init()) {
            Log::AppFatal({0x00, 0x01}, "Failed to initialize application.");
        }

        while (m_IsRunning) {
            Update();

            Render();
        }

        Cleanup();
    }
}
