/* Copyright (C) 2024 Jean "Pixfri" Letessier
* This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Camera.hpp
 * Description : Declaration of the Camera class.
 */
#pragma once

#include <FlashlightEngine/fltypes.hpp>

#include <glm/glm.hpp>

namespace FlashlightEditor {
    class EditorCamera {
        f32 m_LastMouseX = 960;
        f32 m_LastMouseY = 540;

        bool m_EnableMouse = false;
        
    public:
        glm::vec3 Velocity;
        glm::vec3 Position;
        // Vertical rotation
        f32 Pitch = {0.f};
        // Horizontal rotation
        f32 Yaw = {0.f};

        glm::mat4 GetViewMatrix() const;
        glm::mat4 GetRotationMatrix() const;

        void OnKeyDown(i32 keyScancode, f32 deltaTime, f32 speed);
        void OnKeyUp(i32 keyScancode);
        void OnMouseMovement(f32 mouseX, f32 mouseY, f32 sensitivity, f32 deltaTime);

        void Update();
    };
}
