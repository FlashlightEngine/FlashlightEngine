#pragma once

#include "FlashlightEngine/pch.hpp"

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

using customLogCallback = std::function<void(const spdlog::level::level_enum &level, const std::string &msg)>;

class Logger {

    static std::shared_ptr<spdlog::logger> m_EngineLogger;
    static std::shared_ptr<spdlog::logger> m_ApplicationLogger;

public:
    static void Init();
    static inline spdlog::logger *GetEngineLogger();
    static inline spdlog::logger *GetApplicationLogger();
    static void AddEngineCallback(const customLogCallback &callback);
    static void AddApplicationCallback(const customLogCallback &callback);
};


    namespace Log {

        template<typename... Args>
        constexpr void EngineTrace(Args&&... args) {
            Logger::GetEngineLogger()->trace(std::forward<Args>(args)...);
        }

        template<typename... Args>
        constexpr void EngineInfo(Args&&... args) {
            Logger::GetEngineLogger()->info(std::forward<Args>(args)...);
        }

        template<typename... Args>
        constexpr void EngineWarn(Args&&... args) {
            Logger::GetEngineLogger()->warn(std::forward<Args>(args)...);
        }

        template<typename... Args>
        constexpr void EngineError(Args&&... args) {
            Logger::GetEngineLogger()->error(std::forward<Args>(args)...);
        }

        template<typename... Args>
        constexpr void EngineCritical(Args&&... args) {
            Logger::GetEngineLogger()->critical(std::forward<Args>(args)...);
        }


        template<typename... Args>
        constexpr void AppTrace(Args&&... args) {
            Logger::GetApplicationLogger()->trace(std::forward<Args>(args)...);
        }

        template<typename... Args>
        constexpr void AppInfo(Args&&... args) {
            Logger::GetApplicationLogger()->info(std::forward<Args>(args)...);
        }

        template<typename... Args>
        constexpr void AppWarn(Args&&... args) {
            Logger::GetApplicationLogger()->warn(std::forward<Args>(args)...);
        }

        template<typename... Args>
        constexpr void AppError(Args&&... args) {
            Logger::GetApplicationLogger()->error(std::forward<Args>(args)...);
        }

        template<typename... Args>
        constexpr void AppCritical(Args&&... args) {
            Logger::GetApplicationLogger()->critical(std::forward<Args>(args)...);
        }

        template<typename... Args>
        constexpr void AppFatal(const ErrorCode errorCode, Args&&... args) {
            Logger::GetApplicationLogger()->critical(std::forward<Args>(args)...);
            exit(errorCode.GetFormattedErrorCode());
        }
}

#include "Logger.inl"

}