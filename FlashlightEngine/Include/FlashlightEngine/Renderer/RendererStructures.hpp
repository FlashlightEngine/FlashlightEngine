/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : RendererStructures.hpp
 * Description : Contains structures used by the renderer.
 */
#pragma once

#include "FlashlightEngine/Renderer/VulkanWrapper/Buffer.hpp"

#include <glm/glm.hpp>

#include <volk.h>

namespace Flashlight {
    struct UniformBufferObject {
        glm::mat4 Model;
        glm::mat4 View;
        glm::mat4 Projection;
    };
    
    struct FrameObjects {
        VkCommandBuffer FrameCommandBuffer;

        VkSemaphore ImageAvailableSemaphore{};
        VkSemaphore RenderFinishedSemaphore{};
        VkFence InFlightFence{};

        VulkanWrapper::Buffer UniformBuffer;
    };
}
