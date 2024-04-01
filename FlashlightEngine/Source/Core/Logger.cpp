/* Copyright (C) 2024 Jean "Pixfri" Letessier (jean.letessier@protonmail.com)
 * This file is part of "FlashLight Engine"
 * For conditions of distribution and use, see copyright notice in FlashLightEngine.hpp
 *
 * File : Logger.cpp
 * Description This file contains the definitions of methods from the Logger class.
 */

#include "FlashlightEngine/Core/Logger.hpp"

#include "FlashlightEngine/pch.hpp"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/base_sink.h>

namespace Flashlight {

std::shared_ptr<spdlog::logger> Logger::s_EngineLogger;
std::shared_ptr<spdlog::logger> Logger::s_ApplicationLogger;

/// @ingroup Core
/// @class Flashlight::Logger
/// @brief Class that handles logging.

/// @brief Initializes loggers and the default logger pattern.
void Logger::Init() {
    spdlog::set_pattern("%^[%T](%l) %n : %v%$");

    s_EngineLogger = spdlog::stdout_color_mt("FlashLight");
    s_EngineLogger->set_level(spdlog::level::trace);

    s_ApplicationLogger = spdlog::stdout_color_mt("Application");
    s_ApplicationLogger->set_level(spdlog::level::trace);
}

/// @ingroup Core
/// @class Flashlight::CallbackSink
/// @brief Lets the user define a custom callback for the logger.
template<typename Mutex>
class CallbackSink final : public spdlog::sinks::base_sink<Mutex> {
public:
    /// @brief The constructor for the CallbackSink class.
    explicit CallbackSink(LogCallback callback)
        : m_Callback{std::move(callback)} {}

protected:
    /// @brief Formats the message then calls the callback.
    void sink_it_(const spdlog::details::log_msg &msg) override {
        spdlog::memory_buf_t formatted;
        m_Formatter.format(msg, formatted);
        auto eol_len = strlen(spdlog::details::os::default_eol);
        std::string str_msg(formatted.begin(), formatted.end() - eol_len);
        m_Callback(msg.level, str_msg + '\n');
    }

    /// @brief Does nothing.
    void flush_() override {}    
private:
    LogCallback m_Callback;
    spdlog::pattern_formatter m_Formatter;
};

/// @brief Adds a custom callback to the engine logger.
///
/// @param callback The custom callback, of type LogCallback.
void Logger::AddEngineLogCallback(const LogCallback &callback) {
    s_EngineLogger->sinks().push_back(std::make_shared<CallbackSink<std::mutex>>(callback));
}

/// @brief Adds a custom callback to the application logger.
/// 
/// @param callback The custom callback, of type LogCallback.
void Logger::AddApplicationLogCallback(const LogCallback &callback) {
    s_ApplicationLogger->sinks().push_back(std::make_shared<CallbackSink<std::mutex>>(callback));
}

}
