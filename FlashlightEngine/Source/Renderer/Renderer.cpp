/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Renderer.cpp
 * Description : Definitions of methods from the Renderer class.
 */
#include "FlashlightEngine/Renderer/Renderer.hpp"

namespace Flashlight {
    Renderer::Renderer() {
        m_Instance = std::make_unique<WGPUWrapper::Instance>();
        m_Device = std::make_unique<WGPUWrapper::Device>(*m_Instance);
    }


    void Renderer::Initialize() {
    }
}
