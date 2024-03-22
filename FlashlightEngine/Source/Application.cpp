/* Copyright (C) 2024 Jean "Pixfri" Letessier (jean.letessier@protonmail.com)
 * This file is part of "FlashLight Engine"
 * For conditions of distribution and use, see copyright notice in FlashLightEngine.hpp
 *
 * Application.cpp - Definitions of methods from the Application class.
 * This file contains definitions of methods from the Flashlight::Application class.
 */

#include "Application.hpp"

namespace Flashlight {

/// @ingroup Flashlight
/// @class Flashlight::Application
/// @brief The boilerplate class for any application using Flashlight Engine.

/// @brief The first method called at the beginning of the Application.
void Application::Run() {
    Start();
    Update();
}

/// @brief The method called once at the start of the application.
void Application::Start() {}

/// @brief The method called every frame to update the game.
void Application::Update() {
    while (!m_Window->ShouldClose()) {
        m_Window->Update();
    }
}
}