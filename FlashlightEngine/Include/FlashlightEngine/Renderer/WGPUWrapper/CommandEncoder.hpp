/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : CommandEncoder.hpp
 * Description : Declaration of the CommandEncoder class, used to encode commands into command buffers.
 */
#pragma once

#include "FlashlightEngine/Renderer/WGPUWrapper/Device.hpp"

#include "FlashlightEngine/pch.hpp"

namespace Flashlight::WGPUWrapper {
    class CommandEncoder {
        WGPUCommandEncoder m_Encoder = nullptr;

    public:
        explicit inline CommandEncoder(const Device &device);
        inline ~CommandEncoder();

        CommandEncoder(const CommandEncoder&) = default;
        CommandEncoder(CommandEncoder&&) = default;

        CommandEncoder &operator=(const CommandEncoder&) = default;
        CommandEncoder &operator=(CommandEncoder&&) = default;

        [[nodiscard]] inline WGPUCommandEncoder GetNativeEncoder() const;

    private:
        void Create(WGPUDevice device);
        void Destroy() const;
    };

#include "CommandEncoder.inl"
}