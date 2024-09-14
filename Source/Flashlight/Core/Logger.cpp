// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Flashlight/Core/Logger.hpp>

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/base_sink.h>

namespace Flashlight {
    std::shared_ptr<spdlog::logger> Logger::m_Logger;

    void Logger::Init() {
        spdlog::set_pattern("[%H:%M:%S Timezone: %z] [%n] [%^---%L---%$] [Thread #%t] %v");
        m_Logger->set_level(spdlog::level::trace);
    }

    template <typename Mutex>
    class CallbackSink final : public spdlog::sinks::base_sink<Mutex> {
    public:
        explicit CallbackSink(CustomLogCallback callback) : m_Callback(std::move(callback)) {
        }

    protected:
        void sink_it_(const spdlog::details::log_msg& msg) override {
            spdlog::memory_buf_t formatted;
            m_Formatter.format(msg, formatted);
            const auto eolLen = strlen(spdlog::details::os::default_eol);
            const std::string message(formatted.begin(), formatted.end() - eolLen);
            m_Callback(msg.level, message + '\n');
        }

        void flush_() override {
        }

    private:
        CustomLogCallback m_Callback;
        spdlog::pattern_formatter m_Formatter;
    };

    void Logger::AddCallback(const CustomLogCallback& callback) {
        m_Logger->sinks().push_back(std::make_shared<CallbackSink<std::mutex>>(callback));
    }
}
