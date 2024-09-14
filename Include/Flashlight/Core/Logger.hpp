// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef FL_CORE_LOGGER_HPP
#define FL_CORE_LOGGER_HPP

#include <FLUtils/Defines.hpp>
#include <Flashlight/Export.hpp>

#include <spdlog/spdlog.h>

#include <functional>

namespace Flashlight {
    using CustomLogCallback = std::function<void(const spdlog::level::level_enum& level, const std::string& msg)>;

    class FL_API Logger {
        static std::shared_ptr<spdlog::logger> m_Logger;
        
    public:
        static void Init();
        static inline spdlog::logger* GetLogger();
        static void AddCallback(const CustomLogCallback& callback);
    };

    namespace Log {
        template <typename... Args>
        constexpr void Trace(Args&&... args);

        template <typename... Args>
        constexpr void Info(Args&&... args);
        
        template <typename... Args>
        constexpr void Warn(Args&&... args);
        
        template <typename... Args>
        constexpr void Error(Args&&... args);
        
        template <typename... Args>
        constexpr void Critical(Args&&... args);
    }
}

#include <Flashlight/Core/Logger.inl>

#endif // FL_CORE_LOGGER_HPP
