/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : CommandEncoder.cpp
 * Description : Definitions of methods from the CommandEncoder class.
 */#include "FlashlightEngine/Renderer/WGPUWrapper/CommandEncoder.hpp"

#include "FlashlightEngine/pch.hpp"

namespace Flashlight::WGPUWrapper {
    void CommandEncoder::Create(WGPUDevice device) {
        WGPUCommandEncoderDescriptor encoderDescriptor = {};
        encoderDescriptor.nextInChain = nullptr;
        encoderDescriptor.label = "WebGPU Command Encoder";
        m_Encoder = wgpuDeviceCreateCommandEncoder(device, &encoderDescriptor);
    }

    void CommandEncoder::Destroy() const {
        if (m_Encoder) {
            wgpuCommandEncoderRelease(m_Encoder);
        }
    }
}