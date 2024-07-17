#pragma once

inline spdlog::logger *Logger::GetEngineLogger() {
    return m_EngineLogger.get();
}

inline spdlog::logger *Logger::GetApplicationLogger() {
    return m_ApplicationLogger.get();
}