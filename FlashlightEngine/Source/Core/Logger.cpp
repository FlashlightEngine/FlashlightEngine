/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Logger.cpp
 * Description : Definitions of methods from the Logger class.
 */
#include "FlashlightEngine/Core/Logger.hpp"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/base_sink.h>

namespace Flashlight {
    std::shared_ptr<spdlog::logger> Logger::m_EngineLogger;
    std::shared_ptr<spdlog::logger> Logger::m_ApplicationLogger;

    void Logger::Init() {
        spdlog::set_pattern("%^[%T](%l) %n : %v%$");

        m_EngineLogger = spdlog::stdout_color_mt("FlashlightEngine");
        m_EngineLogger->set_level(spdlog::level::trace);

        m_ApplicationLogger = spdlog::stdout_color_mt("Application");
        m_ApplicationLogger->set_level(spdlog::level::trace);
    }

    template <typename Mutex>
    class CallbackSink final : public spdlog::sinks::base_sink<Mutex> {
        customLogCallback m_Callback;
        spdlog::pattern_formatter m_Formatter;

    public:
        explicit CallbackSink(customLogCallback callback) : m_Callback{std::move(callback)} {
        }

    protected:
        void sink_it_(const spdlog::details::log_msg& msg) override {
            spdlog::memory_buf_t formatted;
            m_Formatter.format(msg, formatted);
            const auto eol_len = strlen(spdlog::details::os::default_eol);
            const std::string message(formatted.begin(), formatted.end() - eol_len);
            m_Callback(msg.level, message + '\n');
        }

        void flush_() override {
        }
    };

    void Logger::AddEngineCallback(const customLogCallback& callback) {
        m_EngineLogger->sinks().push_back(std::make_shared<CallbackSink<std::mutex>>(callback));
    }

    void Logger::AddApplicationCallback(const customLogCallback& callback) {
        m_ApplicationLogger->sinks().push_back(std::make_shared<CallbackSink<std::mutex>>(callback));
    }

    namespace Log {
        std::string EvaluateEngineErrorCode(const ErrorCode& errorCode) {
            std::stringstream message;
            switch (errorCode.ModuleId) {
            case 0x00:
                message << "[APPLICATION INITIALIZATION]";
                break;
                
            case 0x01:
                message << "[CORE]";
                break;
                
            case 0x02:
                message << "[RENDERER] ";
                break;

            default:
                message << "[INVALID/UNKNOWN MODULE ID]";
                break;
            }

            return message.str();
        }
    }
}
