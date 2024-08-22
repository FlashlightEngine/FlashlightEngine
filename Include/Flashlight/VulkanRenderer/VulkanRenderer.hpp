// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of the "Flashlight Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef FL_VULKANRENDERER_HPP
#define FL_VULKANRENDERER_HPP

#include <FLUtils/Defines.hpp>
#include <Flashlight/VulkanRenderer/Export.hpp>

namespace Flashlight {
    class FL_VULKANRENDERER_API VulkanRenderer {
    public:
        VulkanRenderer() = default;
        ~VulkanRenderer() = default;

        VulkanRenderer(const VulkanRenderer&) = delete;
        VulkanRenderer(VulkanRenderer&&) = delete;

        VulkanRenderer& operator=(const VulkanRenderer&) = delete;
        VulkanRenderer& operator=(VulkanRenderer&&) = delete;

    private:
    };
}

#include <Flashlight/VulkanRenderer/VulkanRenderer.inl>

#endif // FL_VULKANRENDERER_HPP
