/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Instance.cpp
 * Description : Definitions of methods from the Instance class.
 */
#include "FlashlightEngine/Renderer/WGPUWrapper/Instance.hpp"

#include "FlashlightEngine/Core/Logger.hpp"

namespace Flashlight::WGPUWrapper {
    void Instance::Create() {
        WGPUInstanceDescriptor instanceDescriptor{};
        instanceDescriptor.nextInChain = nullptr;

        m_Instance = wgpuCreateInstance(&instanceDescriptor);

        if (!m_Instance) {
            Log::EngineFatal({0x01, 0x01}, "Failed to create WebGPU instance.");
        }
        Log::EngineTrace("WebGPU instance created.");
    }

    void Instance::Destroy() const {
        if (m_Instance) {
            Log::EngineTrace("Destroying WebGPU instance.");
            wgpuInstanceRelease(m_Instance);
        }
    }
}
