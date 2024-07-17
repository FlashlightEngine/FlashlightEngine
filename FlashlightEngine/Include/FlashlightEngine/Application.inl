#pragma once

inline bool Application::IsRunning() const {
    return m_IsRunning;
}

inline void Application::Close() {
    m_IsRunning = false;
}
