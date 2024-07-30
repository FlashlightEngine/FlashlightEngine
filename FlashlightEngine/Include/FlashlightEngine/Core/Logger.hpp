/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Logger.hpp
 * Description : The logging methods used by the engine and applications made with it.
 */
#pragma once

#include <FlashlightEngine/pch.hpp>

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
        static std::shared_ptr<spdlog::logger> m_ApplicationLogger;

    public:
        static void Init();
        static inline spdlog::logger* GetEngineLogger();
        static inline spdlog::logger* GetApplicationLogger();
        static void AddEngineCallback(const customLogCallback& callback);
        static void AddApplicationCallback(const customLogCallback& callback);
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
        constexpr void AppTrace(Args&&... args);

        template <typename... Args>
        constexpr void AppInfo(Args&&... args);

        template <typename... Args>
        constexpr void AppWarn(Args&&... args);

        template <typename... Args>
        constexpr void AppError(Args&&... args);

        template <typename... Args>
        constexpr void AppCritical(Args&&... args);

        template <typename... Args>
        constexpr void AppFatal(ErrorCode errorCode, Args&&... args);
    }

#include "Logger.inl"
}
