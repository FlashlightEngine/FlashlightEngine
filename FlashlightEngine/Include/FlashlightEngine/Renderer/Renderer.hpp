/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Renderer.hpp
 * Description : Declaration of the Renderer class. It is used to manage rendering objects. Each application has its
 *               own renderer instance.
 */
#pragma once

#include "FlashlightEngine/Renderer/VulkanWrapper/Instance.hpp"

#include "FlashlightEngine/pch.hpp"

namespace Flashlight {
    class Renderer {
        std::unique_ptr<VulkanWrapper::Instance> m_Instance;
        
    public:
        explicit inline Renderer(const DebugLevel &debugLevel);
        ~Renderer() = default;

        Renderer(const Renderer&) = delete;
        Renderer(Renderer&&) = delete;

        Renderer& operator=(const Renderer&) = delete;
        Renderer& operator=(Renderer&&) = delete;
    private:
        void Create(const DebugLevel &debugLevel);
    };

#include "Renderer.inl"
}
