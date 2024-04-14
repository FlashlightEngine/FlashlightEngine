/* Copyright (C) 2024 Jean "Pixfri" Letessier (jean.letessier@protonmail.com)
 * This file is part of "FlashLight Engine"
 * For conditions of distribution and use, see copyright notice in FlashLightEngine.hpp
 *
 * File : Logger.hpp
 * Description : This header file contains the definition of logger functions.
 */

#pragma once

#include "FlashlightEngine/pch.hpp"

#include <spdlog/spdlog.h>

#include <functional>

namespace Flashlight {

using LogCallback = std::function<void(const spdlog::level::level_enum &level, const std::string &msg)>;

class Logger {
public:
    static void Init();
    static inline spdlog::logger *GetEngineLogger();
    static inline spdlog::logger *GetApplicationLogger();
    static void AddEngineLogCallback(const LogCallback &callback);
    static void AddApplicationLogCallback(const LogCallback &callback);

private:
    static std::shared_ptr<spdlog::logger> s_EngineLogger;
    static std::shared_ptr<spdlog::logger> s_ApplicationLogger;
};

namespace Log {
    // Engine log
    template<typename... Args>
    constexpr void EngineTrace(Args&&... args) {
        Logger::GetEngineLogger()->trace(std::forward<Args>(args));
    }
    
    template<typename... Args>
    constexpr void EngineInfo(Args&&... args) {
        Logger::GetEngineLogger()->info(std::forward<Args>(args));
    }
    
    template<typename... Args>
    constexpr void EngineWarning(Args&&... args) {
        Logger::GetEngineLogger()->warn(std::forward<Args>(args));
    }
    
    template<typename... Args>
    constexpr void EngineError(Args&&... args) {
        Logger::GetEngineLogger()->error(std::forward<Args>(args));
    }
    
    // Application log
    template<typename... Args>
    constexpr void Trace(Args&&... args) {
        Logger::GetApplicationLogger()->trace(std::forward<Args>(args));
    }
    
    template<typename... Args>
    constexpr void Info(Args&&... args) {
        Logger::GetApplicationLogger()->info(std::forward<Args>(args));
    }
    
    template<typename... Args>
    constexpr void Warning(Args&&... args) {
        Logger::GetApplicationLogger()->warn(std::forward<Args>(args));
    }
    
    template<typename... Args>
    constexpr void Error(Args&&... args) {
        Logger::GetApplicationLogger()->error(std::forward<Args>(args));
    }
}

#include "Logger.inl"

}
