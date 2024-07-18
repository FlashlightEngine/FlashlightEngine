/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : CommandEncoder.inl
 * Description : Definitions of inline methods from the CommandEncoder class.
 */
#pragma once

inline CommandEncoder::CommandEncoder(const Device &device) {
    Create(device.GetNativeDevice());
}

inline CommandEncoder::~CommandEncoder() {
    Destroy();
}

inline WGPUCommandEncoder CommandEncoder::GetNativeEncoder() const {
    return m_Encoder;
}
