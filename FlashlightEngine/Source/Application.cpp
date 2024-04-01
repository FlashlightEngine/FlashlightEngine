/* Copyright (C) 2024 Jean "Pixfri" Letessier (jean.letessier@protonmail.com)
 * This file is part of "FlashLight Engine"
 * For conditions of distribution and use, see copyright notice in FlashLightEngine.hpp
 *
 * File : Application.cpp
 * Description : This file contains definitions of methods from the Flashlight::Application class.
 */

#include "FlashlightEngine/Application.hpp"

namespace Flashlight {

/// @ingroup Flashlight
/// @class Flashlight::Application
/// @brief The boilerplate class for any application using Flashlight Engine.

Application *Application::s_Application = nullptr;

/// @brief Constructor for the Application class.
Application::Application() {    
    if (s_Application) {
        Log::EngineError("Application already running.");
        throw std::runtime_error("You can only run this application once.");
    }
}

/// @brief The first method called at the beginning of the Application.
void Application::Run() {
    while (m_Running) {
        m_Window->Update();

        Update();

        Render();

        if (m_Window->ShouldClose()) {
            m_Running = false;
        }
    }
}

}