/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Window.inl
 * Description : Definitions of inline methods from the Window class.
 */
#pragma once

inline Window::~Window() {

}

inline bool Window::ShouldClose() const {
}

inline i32 Window::GetWidth() const {
    return m_Data.Width;
}

inline i32 Window::GetHeight() const {
    return m_Data.Height;
}

inline void Window::Update() {
}

inline void Window::Close() const {
}
