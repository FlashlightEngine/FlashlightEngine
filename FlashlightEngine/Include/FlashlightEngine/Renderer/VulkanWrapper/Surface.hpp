/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Surface.hpp
 * Description : Declaration of the Surface class. The surface abstracts the OS native window/surface.
 */
#pragma once

#include "FlashlightEngine/Renderer/VulkanWrapper/Instance.hpp"

#include "FlashlightEngine/Core/Window.hpp"

namespace Flashlight::VulkanWrapper {
    class Surface {
        VkSurfaceKHR m_Surface = VK_NULL_HANDLE;

        VkInstance m_Instance = VK_NULL_HANDLE;
        GLFWwindow* m_Window = nullptr;

    public:
        explicit inline Surface(const Instance& instance, const Window& window);
        inline ~Surface();

        Surface(const Surface&) = delete;
        Surface(Surface&&) = delete;

        Surface& operator=(const Surface&) = delete;
        Surface& operator=(Surface&&) = delete;

        [[nodiscard]] inline VkSurfaceKHR GetNativeSurface() const;

    private:
        void Create();
        inline void Destroy() const;
    };

#include "Surface.inl"
}
