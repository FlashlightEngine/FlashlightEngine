/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Logger.hpp
 * Description : The logging methods used by the engine and applications made with it.
 */
#pragma once

#include <FlashlightEngine/flpch.hpp>

#include <spdlog/spdlog.h>

namespace Flashlight {
    /*
     * ErrorCode : The representation of an error code when throwing an error.
     * The first byte in the error code represents the module identifier.
     * The last byte in the error code represents the error number in that module.
     */
    struct ErrorCode {
        u8 ModuleId;
        u8 ErrorNumber;

        [[nodiscard]]
        u16 GetFormattedErrorCode() const {
            return (ModuleId << 8) + ErrorNumber;
        }
    };

    using customLogCallback = std::function<void(const spdlog::level::level_enum& level, const std::string& msg)>;

    class Logger {
        static std::shared_ptr<spdlog::logger> m_EngineLogger;
        static std::shared_ptr<spdlog::logger> m_EditorLogger;

    public:
        static void Init();
        static inline spdlog::logger* GetEngineLogger();
        static inline spdlog::logger* GetEditorLogger();
        static void AddEngineCallback(const customLogCallback& callback);
        static void AddEditorCallback(const customLogCallback& callback);
    };


    namespace Log {
        std::string EvaluateEngineErrorCode(const ErrorCode& errorCode); 
        
        template <typename... Args>
        constexpr void EngineTrace(Args&&... args);

        template <typename... Args>
        constexpr void EngineInfo(Args&&... args);

        template <typename... Args>
        constexpr void EngineWarn(Args&&... args);

        template <typename... Args>
        constexpr void EngineError(Args&&... args);

        template <typename... Args>
        constexpr void EngineCritical(Args&&... args);

        template <typename... Args>
        constexpr void EngineFatal(ErrorCode errorCode, Args&&... args);


        template <typename... Args>
        constexpr void EditorTrace(Args&&... args);

        template <typename... Args>
        constexpr void EditorInfo(Args&&... args);

        template <typename... Args>
        constexpr void EditorWarn(Args&&... args);

        template <typename... Args>
        constexpr void EditorError(Args&&... args);

        template <typename... Args>
        constexpr void EditorCritical(Args&&... args);

        template <typename... Args>
        constexpr void EditorFatal(ErrorCode errorCode, Args&&... args);
    }

#include "Logger.inl"
}
