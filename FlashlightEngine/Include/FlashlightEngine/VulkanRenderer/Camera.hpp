/* Copyright (C) 2024 Jean "Pixfri" Letessier
* This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Camera.hpp
 * Description : Declaration of the Camera class.
 */
#pragma once

#include <FlashlightEngine/VulkanRenderer/VulkanTypes.hpp>

#include <FlashlightEngine/Core/Window.hpp>

namespace Flashlight::VulkanRenderer {
    class Camera {
    public:
        glm::vec3 Velocity;
        glm::vec3 Position;
        // Vertical rotation
        f32 Pitch = {0.f};
        // Horizontal rotation
        f32 Yaw = {0.f};

        glm::mat4 GetViewMatrix() const;
        glm::mat4 GetRotationMatrix() const;

        void ProcessInput(const Window& window, f32 deltaTime);

        void Update();
    };
}
