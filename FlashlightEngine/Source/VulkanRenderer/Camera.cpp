/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Camera.cpp
 * Description : Definitions of methods from the Camera class.
 */
#include <FlashlightEngine/VulkanRenderer/Camera.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Flashlight::VulkanRenderer {
    glm::mat4 Camera::GetViewMatrix() const {
        // To create a correct model view, the world needs to be moved in the opposite direction of the camera.
        const glm::mat4 cameraTranslation = glm::translate(glm::mat4(1.f), Position);
        const glm::mat4 cameraRotation = GetRotationMatrix();
        return glm::inverse(cameraTranslation * cameraRotation);
    }

    glm::mat4 Camera::GetRotationMatrix() const {
        const glm::quat pitchRotation = glm::angleAxis(Pitch, glm::vec3{1.f, 0.f, 0.f});
        const glm::quat yawRotation = glm::angleAxis(Yaw, glm::vec3(0.f, 1.f, 0.f));

        return glm::toMat4(yawRotation) * glm::toMat4(pitchRotation);
    }

    void Camera::ProcessInput(const Window& window, const f32 deltaTime, const f32 speed) {
        if (window.GetKeyState(Keys::W) == KeyState::Released) {
            Velocity.z = 0;
        }

        if (window.GetKeyState(Keys::S) == KeyState::Released) {
            Velocity.z = 0;
        }

        if (window.GetKeyState(Keys::A) == KeyState::Released) {
            Velocity.x = 0;
        }

        if (window.GetKeyState(Keys::D) == KeyState::Released) {
            Velocity.x = 0;
        }

        // Camera movement
        if (window.GetKeyState(Keys::W) == KeyState::Pressed) {
            Velocity.z = -speed * deltaTime;
        }
        
        if (window.GetKeyState(Keys::S) == KeyState::Pressed) {
            Velocity.z = speed * deltaTime;
        }
        
        if (window.GetKeyState(Keys::A) == KeyState::Pressed) {
            Velocity.x = -speed * deltaTime;
        }
        
        if (window.GetKeyState(Keys::D) == KeyState::Pressed) {
            Velocity.x = speed * deltaTime;
        }

        // Camera rotation
        if (window.GetKeyState(Keys::Up) == KeyState::Pressed) {
            Pitch += 1.5f * deltaTime;
        }

        if (window.GetKeyState(Keys::Down) == KeyState::Pressed) {
            Pitch -= 1.5f * deltaTime;
        }

        if (window.GetKeyState(Keys::Left) == KeyState::Pressed) {
            Yaw += 2.f * deltaTime;
        }

        if (window.GetKeyState(Keys::Right) == KeyState::Pressed) {
            Yaw -= 2.f * deltaTime;
        }
    }

    void Camera::Update() {
        const glm::mat4 cameraRotation = GetRotationMatrix();
        Position += glm::vec3(cameraRotation * glm::vec4(Velocity, 0.f));
    }
}
