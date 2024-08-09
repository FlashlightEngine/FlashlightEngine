/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Camera.cpp
 * Description : Definitions of methods from the Camera class.
 */
#include <FlashlightEditor/EditorCamera.hpp>

#include <FlashlightEngine/Core/Window.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace FlashlightEditor {
    glm::mat4 EditorCamera::GetViewMatrix() const {
        // To create a correct model view, the world needs to be moved in the opposite direction of the camera.
        const glm::mat4 cameraTranslation = glm::translate(glm::mat4(1.f), Position);
        const glm::mat4 cameraRotation = GetRotationMatrix();
        return glm::inverse(cameraTranslation * cameraRotation);
    }

    glm::mat4 EditorCamera::GetRotationMatrix() const {
        const glm::quat pitchRotation = glm::angleAxis(Pitch, glm::vec3{1.f, 0.f, 0.f});
        const glm::quat yawRotation = glm::angleAxis(Yaw, glm::vec3(0.f, 1.f, 0.f));

        return glm::toMat4(yawRotation) * glm::toMat4(pitchRotation);
    }

    void EditorCamera::OnKeyDown(const i32 keyScancode, const f32 deltaTime, const f32 speed) {
        // Camera movement
        if (keyScancode == static_cast<i32>(Flashlight::Keys::W)) {
            Velocity.z = -speed * deltaTime;
        }
        
        if (keyScancode == static_cast<i32>(Flashlight::Keys::S)) {
            Velocity.z = speed * deltaTime;
        }
        
        if (keyScancode == static_cast<i32>(Flashlight::Keys::A)) {
            Velocity.x = -speed * deltaTime;
        }
        
        if (keyScancode == static_cast<i32>(Flashlight::Keys::D)) {
            Velocity.x = speed * deltaTime;
        }

        if (keyScancode == static_cast<i32>(Flashlight::Keys::Q)) {
            m_EnableMouse = true;
        }
    }

    void EditorCamera::OnKeyUp(const i32 keyScancode) {
        if (keyScancode == static_cast<i32>(Flashlight::Keys::W)) {
            Velocity.z = 0;
        }

        if (keyScancode == static_cast<i32>(Flashlight::Keys::S)) {
            Velocity.z = 0;
        }

        if (keyScancode == static_cast<i32>(Flashlight::Keys::A)) {
            Velocity.x = 0;
        }

        if (keyScancode == static_cast<i32>(Flashlight::Keys::D)) {
            Velocity.x = 0;
        }

        if (keyScancode == static_cast<i32>(Flashlight::Keys::Q)) {
            m_EnableMouse = false;
        }
    }

    void EditorCamera::OnMouseMovement(const f32 mouseX, const f32 mouseY, const f32 sensitivity, const f32 deltaTime) {
        f32 xOffset = m_LastMouseX - mouseX;
        f32 yOffset = m_LastMouseY - mouseY;
        m_LastMouseX = mouseX;
        m_LastMouseY = mouseY;

        if (m_EnableMouse) {
            xOffset *= (sensitivity * deltaTime);
            yOffset *= (sensitivity * deltaTime);

            Yaw += xOffset;
            Pitch += yOffset;
        }
    }

    void EditorCamera::Update() {
        const glm::mat4 cameraRotation = GetRotationMatrix();
        Position += glm::vec3(cameraRotation * glm::vec4(Velocity, 0.f));
    }
}
