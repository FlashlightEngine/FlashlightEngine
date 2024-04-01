/* Copyright (C) 2024 Jean "Pixfri" Letessier (jean.letessier@protonmail.com)
 * This file is part of "FlashLight Engine"
 * For conditions of distribution and use, see copyright notice in FlashLightEngine.hpp
 *
 * File : Logger.inl
 * Description : This file contains the definitions of inline methods from the Logger class.
 */

#pragma once

/// @ingroup Core
/// @class Flashlight::Logger
/// @brief Class that handles base logging.

/// @brief Returns the engine's logger.
inline spdlog::logger *Logger::GetEngineLogger() {
    return s_EngineLogger.get();
}

/// @brief Returns the application logger.
inline spdlog::logger *Logger::GetApplicationLogger() {
    return s_EngineLogger.get();
}
