/* Copyright (C) 2024 Jean "Pixfri" Letessier (jean.letessier@protonmail.com)
 * This file is part of "FlashLight Engine"
 * For conditions of distribution and use, see copyright notice in FlashLightEngine.hpp
 *
 * File : Application.inl
 * Description : This file contains the definitions of methods from the Application class.
 */

#pragma once

/// @ingroup Flashlight
/// @class Flashlight::Application
/// @brief The boilerplate class for any application using Flashlight Engine.

/// @brief Gets the application instance.
///
/// @returns The application instance.
inline Application &Application::Get() noexcept {
    return *s_Application;
}

/// @brief Returns the application window.
///
/// @returns The application window.
inline Window Application::GetWindow() const noexcept {
    return *m_Window;
}