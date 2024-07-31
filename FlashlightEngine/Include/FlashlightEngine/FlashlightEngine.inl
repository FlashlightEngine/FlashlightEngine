/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Application.inl
 * Description : Definitions of inline methods from the Application class.
 */
#pragma once

inline FlashlightEngine& FlashlightEngine::GetRunningInstance() {
    return *m_SLoadedApplication;
}

inline bool FlashlightEngine::IsRunning() const {
    return m_IsRunning;
}

inline void FlashlightEngine::Close() {
    m_IsRunning = false;
}
