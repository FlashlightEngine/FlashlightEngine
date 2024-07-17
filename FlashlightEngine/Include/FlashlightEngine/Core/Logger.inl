#pragma once

inline spdlog::logger* Logger::GetEngineLogger() {
    return m_EngineLogger.get();
}

inline spdlog::logger* Logger::GetApplicationLogger() {
    return m_ApplicationLogger.get();
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
        Logger::GetEngineLogger()->critical(std::forward<Args>(args)...);
        exit(errorCode.GetFormattedErrorCode());
    }


    template <typename... Args>
    constexpr void AppTrace(Args&&... args) {
        Logger::GetApplicationLogger()->trace(std::forward<Args>(args)...);
    }

    template <typename... Args>
    constexpr void AppInfo(Args&&... args) {
        Logger::GetApplicationLogger()->info(std::forward<Args>(args)...);
    }

    template <typename... Args>
    constexpr void AppWarn(Args&&... args) {
        Logger::GetApplicationLogger()->warn(std::forward<Args>(args)...);
    }

    template <typename... Args>
    constexpr void AppError(Args&&... args) {
        Logger::GetApplicationLogger()->error(std::forward<Args>(args)...);
    }

    template <typename... Args>
    constexpr void AppCritical(Args&&... args) {
        Logger::GetApplicationLogger()->critical(std::forward<Args>(args)...);
    }

    template <typename... Args>
    constexpr void AppFatal(const ErrorCode errorCode, Args&&... args) {
        Logger::GetApplicationLogger()->critical(std::forward<Args>(args)...);
        exit(errorCode.GetFormattedErrorCode());
    }
}
