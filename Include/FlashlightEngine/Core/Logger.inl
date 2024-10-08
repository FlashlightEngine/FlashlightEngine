﻿// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in LICENSE

#pragma once

inline spdlog::logger* Logger::GetEngineLogger() {
    return m_EngineLogger.get();
}

inline spdlog::logger* Logger::GetEditorLogger() {
    return m_EditorLogger.get();
}

namespace Log {
    template <typename... Args>
    constexpr void EngineTrace(Args&&... args) {
        Logger::GetEngineLogger()->trace(std::forward<Args>(args)...);
    }

    template <typename... Args>
    constexpr void EngineInfo(Args&&... args) {
        Logger::GetEngineLogger()->info(std::forward<Args>(args)...);
    }

    template <typename... Args>
    constexpr void EngineWarn(Args&&... args) {
        Logger::GetEngineLogger()->warn(std::forward<Args>(args)...);
    }

    template <typename... Args>
    constexpr void EngineError(Args&&... args) {
        Logger::GetEngineLogger()->error(std::forward<Args>(args)...);
    }

    template <typename... Args>
    constexpr void EngineCritical(Args&&... args) {
        Logger::GetEngineLogger()->critical(std::forward<Args>(args)...);
    }

    template <typename... Args>
    constexpr void EngineFatal(const ErrorCode errorCode, Args&&... args) {
        std::stringstream hexErrorCodeStream;
        hexErrorCodeStream << std::hex << errorCode.GetFormattedErrorCode();
        Logger::GetEngineLogger()->critical("{0} FATAL ENGINE ERROR: Code: 0x{1}",
                                                 EvaluateEngineErrorCode(errorCode), hexErrorCodeStream.str());
        Logger::GetEngineLogger()->critical(std::forward<Args>(args)...);
        exit(errorCode.GetFormattedErrorCode());
    }


    template <typename... Args>
    constexpr void EditorTrace(Args&&... args) {
        Logger::GetEditorLogger()->trace(std::forward<Args>(args)...);
    }

    template <typename... Args>
    constexpr void EditorInfo(Args&&... args) {
        Logger::GetEditorLogger()->info(std::forward<Args>(args)...);
    }

    template <typename... Args>
    constexpr void EditorWarn(Args&&... args) {
        Logger::GetEditorLogger()->warn(std::forward<Args>(args)...);
    }

    template <typename... Args>
    constexpr void EditorError(Args&&... args) {
        Logger::GetEditorLogger()->error(std::forward<Args>(args)...);
    }

    template <typename... Args>
    constexpr void EditorCritical(Args&&... args) {
        Logger::GetEditorLogger()->critical(std::forward<Args>(args)...);
    }

    template <typename... Args>
    constexpr void EditorFatal(const ErrorCode errorCode, Args&&... args) {
        std::stringstream hexErrorCodeStream;
        hexErrorCodeStream << std::hex << errorCode.GetFormattedErrorCode();
        Logger::GetEditorLogger()->critical("FATAL EDITOR ERROR: Code: 0x{0}", hexErrorCodeStream.str());
        Logger::GetEditorLogger()->critical(std::forward<Args>(args)...);
        exit(errorCode.GetFormattedErrorCode());
    }
}
