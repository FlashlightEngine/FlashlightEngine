/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Renderer.hpp
 * Description : Declaration of the Renderer class. This class manages objects required for rendering, such as
 *               the WebGPU instance and device.
 */
#pragma once

#include "FlashlightEngine/Renderer/WGPUWrapper/Instance.hpp"
#include "FlashlightEngine/Renderer/WGPUWrapper/Device.hpp"
#include "FlashlightEngine/Renderer/WGPUWrapper/Queue.hpp"

#include "FlashlightEngine/pch.hpp"

namespace Flashlight {
    class Renderer {
        std::unique_ptr<WGPUWrapper::Instance> m_Instance;
        std::unique_ptr<WGPUWrapper::Device> m_Device;
        std::unique_ptr<WGPUWrapper::Queue> m_Queue;

    public:
        Renderer();
        ~Renderer() = default;

        static void Initialize();

        [[nodiscard]] inline WGPUWrapper::Device GetDevice() const;
        [[nodiscard]] inline WGPUWrapper::Queue GetQueue() const;
    };

#include "Renderer.inl"
}
