/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Window.inl
 * Description : Definitions of inline methods from the Window class.
 */
#pragma once

inline bool Window::ShouldClose() const {
    return m_Data.ShouldClose;
}

inline bool Window::ShouldStopRendering() const {
    return m_Data.StopRendering;
}

inline SDL_Window* Window::GetNativeWindow() const {
    return m_Window;
}

inline i32 Window::GetWidth() const {
    return m_Data.Width;
}

inline i32 Window::GetHeight() const {
    return m_Data.Height;
}

inline void Window::Close() {
    m_Data.ShouldClose = true;
}
