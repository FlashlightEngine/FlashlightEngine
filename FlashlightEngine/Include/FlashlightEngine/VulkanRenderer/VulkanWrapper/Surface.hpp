/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Surface.hpp
 * Description : Declaration of the Surface class. It is used create a Vulkan window surface.
 */
#pragma once

#include <FlashlightEngine/VulkanRenderer/VulkanWrapper/Instance.hpp>

#include <FlashlightEngine/Core/Window.hpp>

namespace Flashlight::VulkanRenderer::VulkanWrapper {
    class Surface {
        VkSurfaceKHR m_Surface = VK_NULL_HANDLE; // Vulkan window surface.

        std::shared_ptr<Instance> m_Instance;

    public:
        Surface(const std::shared_ptr<Instance>& instance, const Window& window);
        ~Surface();

        Surface(const Surface&) = delete;
        Surface(Surface&& other) noexcept;

        Surface& operator=(const Surface&) = delete;
        Surface& operator=(Surface&& other) noexcept;

        [[nodiscard]] inline VkSurfaceKHR GetSurface() const;
    };

#include "Surface.inl"
}
