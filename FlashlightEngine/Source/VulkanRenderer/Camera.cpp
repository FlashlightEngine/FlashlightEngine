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

    void Camera::ProcessSDLEvent(const SDL_Event& e, const f32 deltaTime) {
        if (e.type == SDL_KEYDOWN) {
            if (e.key.keysym.sym == SDLK_w) {
                Velocity.z = -1 * deltaTime;
            }
            if (e.key.keysym.sym == SDLK_s) {
                Velocity.z = 1 * deltaTime;
            }
            if (e.key.keysym.sym == SDLK_a) {
                Velocity.x = -1 * deltaTime;
            }
            if (e.key.keysym.sym == SDLK_d) {
                Velocity.x = 1 * deltaTime;
            }
        }

        if (e.type == SDL_KEYUP) {
            if (e.key.keysym.sym == SDLK_w) {
                Velocity.z = 0;
            }
            if (e.key.keysym.sym == SDLK_s) {
                Velocity.z = 0;
            }
            if (e.key.keysym.sym == SDLK_a) {
                Velocity.x = 0;
            }
            if (e.key.keysym.sym == SDLK_d) {
                Velocity.x = 0;
            }
        }

        if (e.type == SDL_MOUSEMOTION) {
            Yaw += static_cast<f32>(e.motion.xrel * -1) * deltaTime;
            Pitch += static_cast<f32>(e.motion.yrel * -1) * deltaTime;
        }
    }

    void Camera::Update() {
        const glm::mat4 cameraRotation = GetRotationMatrix();
        Position += glm::vec3(cameraRotation * glm::vec4(Velocity, 0.f));
    }
}
