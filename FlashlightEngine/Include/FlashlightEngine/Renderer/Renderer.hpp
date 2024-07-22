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
#include "FlashlightEngine/Renderer/VulkanWrapper/DebugMessenger.hpp"
#include "FlashlightEngine/Renderer/VulkanWrapper/Surface.hpp"
#include "FlashlightEngine/Renderer/VulkanWrapper/Device.hpp"
#include "FlashlightEngine/Renderer/VulkanWrapper/SwapChain.hpp"

namespace Flashlight {
    class Renderer {
        std::unique_ptr<VulkanWrapper::Instance> m_Instance;
        std::unique_ptr<VulkanWrapper::DebugMessenger> m_DebugMessenger;
        std::unique_ptr<VulkanWrapper::Surface> m_Surface;
        std::unique_ptr<VulkanWrapper::Device> m_Device;
        std::unique_ptr<VulkanWrapper::SwapChain> m_SwapChain;
        
    public:
        inline Renderer(const DebugLevel &debugLevel, const Window& window);
        ~Renderer() = default;

        Renderer(const Renderer&) = delete;
        Renderer(Renderer&&) = delete;

        Renderer& operator=(const Renderer&) = delete;
        Renderer& operator=(Renderer&&) = delete;
    private:
        void Create(const DebugLevel &debugLevel, const Window& window);
    };

#include "Renderer.inl"
}
