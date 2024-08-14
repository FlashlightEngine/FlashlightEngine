/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : VulkanTypes.hpp
 * Description : Header which includes different Vulkan headers used across the whole renderer, as well as a macro
 *               to check the return status of vulkan functions.
 */
#pragma once

#include <FlashlightEngine/flpch.hpp>
#include <FlashlightEngine/Core/Logger.hpp>

#include <vulkan/vulkan.h>
// ReSharper disable once CppUnusedIncludeDirective
#include <vulkan/vk_enum_string_helper.h>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

#include <deque>
#include <functional>
#include <ranges>

namespace Flashlight {
    enum class DebugLevel {
        None = 0,

        Errors = 1,
        Warnings = 2,
        Verbose = 3,
        Debug = 4
    };

    namespace Renderer {
        struct DeletionQueue {
            std::deque<std::function<void()>> Deletors;

            void PushFunction(std::function<void()>&& function) {
                Deletors.push_back(std::move(function));
            }

            void Flush() {
                // Reverse iterate the deletion queue to execute all the functions.
                for (auto& deletor : std::ranges::reverse_view(Deletors)) {
                    deletor();
                }

                Deletors.clear();
            }
        };

        struct Vertex {
            glm::vec3 Position;
            f32 UvX;
            glm::vec3 Normal;
            f32 UvY;
            glm::vec4 Color;
        };

        // Push constants for mesh objects draws
        struct GPUDrawPushConstants {
            glm::mat4 WorldMatrix;
            VkDeviceAddress VertexBuffer;
        };

        struct GPUSceneData {
            glm::mat4 View;
            glm::mat4 Projection;
            glm::mat4 ViewProjection;
            glm::vec4 AmbientColor;
            glm::vec4 SunlightDirection; // w for light intensity.
            glm::vec4 SunlightColor;
        };

        struct DrawContext;

        // Base class for a renderable dynamic object.
        class IRenderable {
        public:
            virtual ~IRenderable() = default;

        private:
            virtual void Draw(const glm::mat4& topMatrix, DrawContext& context) = 0;
        };

        // Implementation of a drawable scene node.
        // The scene node can hold children and will also keep a transform to propagate to them.
        struct Node : IRenderable {
            // Parent pointer must be a weak pointer to avoid circular dependencies.
            std::weak_ptr<Node> Parent;
            std::vector<std::shared_ptr<Node>> Children;

            glm::mat4 LocalTransform;
            glm::mat4 WorldTransform;

            void RefreshTransform(const glm::mat4& parentMatrix);
            void Draw(const glm::mat4& topMatrix, DrawContext& context) override;
        };
    }
}

#define VK_CHECK(x)                                                                 \
    do {                                                                            \
        VkResult err = x;                                                           \
        if (err) {                                                                  \
            Flashlight::Log::EngineError("Vulkan Error: {}", string_VkResult(err)); \
            abort();                                                                \
        }                                                                           \
    } while (0);
