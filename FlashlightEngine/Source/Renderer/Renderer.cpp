/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Renderer.cpp
 * Description : Definitions of methods from the Renderer class.
 */
#include "FlashlightEngine/Renderer/Renderer.hpp"

#include "FlashlightEngine/Renderer/VulkanWrapper/Instance.hpp"

#include "FlashlightEngine/pch.hpp"

namespace Flashlight {
    void Renderer::Create() {
        m_Instance = std::make_unique<VulkanWrapper::Instance>();
    }

}
