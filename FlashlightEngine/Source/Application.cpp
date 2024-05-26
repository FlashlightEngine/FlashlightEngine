/* Copyright (C) 2024 Jean "Pixfri" Letessier (jean.letessier@protonmail.com)
 * This file is part of "FlashLight Engine"
 * For conditions of distribution and use, see copyright notice in FlashLightEngine.hpp
 *
 * File : Application.cpp
 * Description : This file contains definitions of methods from the Flashlight::Application class.
 */

#include "FlashlightEngine/Application.hpp"

#include "FlashlightEngine/Core/Log.hpp"

namespace Flashlight {

/// @ingroup Flashlight
/// @class Flashlight::Application
/// @brief The boilerplate class for any application using Flashlight Engine.

/// @brief Constructor for the Application class.
Application::Application() {
}

/// @brief The first method called at the beginning of the Application.
void Application::Run() {
    FL_LOG("Starting application")
    Start();
    
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