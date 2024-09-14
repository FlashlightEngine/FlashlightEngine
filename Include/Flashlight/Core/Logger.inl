// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

namespace Flashlight {
    inline spdlog::logger* Logger::GetLogger() {
        return m_Logger.get();
    }

    namespace Log {
        template <typename... Args>
        constexpr void Trace(Args&&... args) {
            Logger::GetLogger()->trace(std::forward<Args>(args)...);
        }

        template <typename... Args>
        constexpr void Info(Args&&... args) {
            Logger::GetLogger()->info(std::forward<Args>(args)...);
        }

        template <typename... Args>
        constexpr void Warn(Args&&... args) {
            Logger::GetLogger()->warn(std::forward<Args>(args)...);
        }

        template <typename... Args>
        constexpr void Error(Args&&... args) {
            Logger::GetLogger()->error(std::forward<Args>(args)...);
        }

        template <typename... Args>
        constexpr void Critical(Args&&... args) {
            Logger::GetLogger()->critical(std::forward<Args>(args)...);
        }
    }
}
