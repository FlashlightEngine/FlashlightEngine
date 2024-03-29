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