/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Renderer.cpp
 * Description : Definitions of methods from the Renderer class.
 */
#include "FlashlightEngine/Renderer/Renderer.hpp"

#include "FlashlightEngine/Renderer/VulkanWrapper/Instance.hpp"

namespace Flashlight {
    void Renderer::Create(const DebugLevel& debugLevel, const Window& window) {
        m_Instance = std::make_unique<VulkanWrapper::Instance>(debugLevel);

        if (debugLevel > DebugLevel::None) {
            m_DebugMessenger = std::make_unique<VulkanWrapper::DebugMessenger>(*m_Instance, debugLevel);
        }

        m_Surface = std::make_unique<VulkanWrapper::Surface>(*m_Instance, window);

        m_Device = std::make_unique<VulkanWrapper::Device>(*m_Instance, *m_Surface, debugLevel);

        m_SwapChain = std::make_unique<VulkanWrapper::SwapChain>(*m_Device, window, *m_Surface);
    }

    std::unique_ptr<GraphicsPipeline> Renderer::CreatePipeline(
        const std::filesystem::path& vertexShaderPath,
        const std::filesystem::path& fragmentShaderPath) const {
        return std::make_unique<GraphicsPipeline>(*m_Device, vertexShaderPath, fragmentShaderPath);
    }

}
