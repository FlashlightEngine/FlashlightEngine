// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in LICENSE

#pragma once

inline Application& Application::GetRunningInstance() {
    return *m_SLoadedApplication;
}

inline bool Application::IsRunning() const {
    return m_IsRunning;
}

inline void Application::Close() {
    m_IsRunning = false;
}
