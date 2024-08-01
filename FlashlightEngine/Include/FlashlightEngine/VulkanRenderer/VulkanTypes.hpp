/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : VulkanTypes.hpp
 * Description : Header which includes different Vulkan headers used across the whole renderer, as well as a macro
 *               to check the return status of vulkan functions.
 */
#pragma once

#include <FlashlightEngine/pch.hpp>
#include <FlashlightEngine/Core/Logger.hpp>

#include <vulkan/vulkan.h>
// ReSharper disable once CppUnusedIncludeDirective
#include <vulkan/vk_enum_string_helper.h>
#include <vk_mem_alloc.h>

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <ranges>

namespace Flashlight {
    enum class DebugLevel {
        None = 0,

        Errors = 1,
        Warnings = 2,
        Verbose = 3,
        Debug = 4
    };

    namespace VulkanRenderer {
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

        struct AllocatedImage {
            VkImage Image;
            VkImageView ImageView;
            VmaAllocation Allocation;
            VkExtent3D ImageExtent;
            VkFormat ImageFormat;
        };

        struct AllocatedBuffer {
            VkBuffer Buffer;
            VmaAllocation Allocation;
            VmaAllocationInfo Info;
        };

        struct FrameData {
            VkCommandPool CommandPool;
            VkCommandBuffer MainCommandBuffer;

            VkSemaphore SwapchainSemaphore, RenderSemaphore;
            VkFence RenderFence;

            u32 SwapchainImageIndex;

            DeletionQueue DeletionQueue;
        };

        struct Vertex {
            glm::vec3 Position;
            f32 UvX;
            glm::vec3 Normal;
            f32 UvY;
            glm::vec4 Color;
        };

        // Holds the resources needed for a mesh.
        struct GPUMeshBuffers {
            AllocatedBuffer IndexBuffer;
            AllocatedBuffer VertexBuffer;
            VkDeviceAddress VertexBufferAddress;
        };

        // Push constants for mesh objects draws
        struct GPUDrawPushConstants {
            glm::mat4 WorldMatrix;
            VkDeviceAddress VertexBuffer;
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
