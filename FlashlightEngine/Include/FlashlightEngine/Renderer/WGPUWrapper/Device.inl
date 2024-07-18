/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Device.inl
 * Description : Definitions of inline methods from the Device class.
 */
#pragma once

inline WGPUDevice Device::GetNativeDevice() const {
    return m_Device;
}
