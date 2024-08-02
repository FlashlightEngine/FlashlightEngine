/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : VulkanDescriptors.hpp
 * Description : Abstraction of Vulkan descriptor sets.
 */
#pragma once

#include <FlashlightEngine/VulkanRenderer/VulkanTypes.hpp>

#include <span>

namespace Flashlight::VulkanRenderer {
#pragma region Descriptor Layout Builder
    struct DescriptorLayoutBuilder {
        std::vector<VkDescriptorSetLayoutBinding> Bindings;

        void AddBinding(u32 binding, VkDescriptorType type);
        void Clear();
        VkDescriptorSetLayout Build(VkDevice device, VkShaderStageFlags shaderStages, const void* pNext = nullptr,
                                    VkDescriptorSetLayoutCreateFlags flags = 0);
    };
#pragma endregion Descriptor Layout Builder

#pragma region Descriptor Pool
    struct DescriptorAllocator {
        struct PoolSizeRatio {
            VkDescriptorType Type;
            f32 Ratio;
        };

        VkDescriptorPool Pool;

        void InitPool(VkDevice device, u32 maxSets, std::span<PoolSizeRatio> poolRatios);
        void ClearDescriptors(VkDevice device) const;
        void DestroyPool(VkDevice device) const;

        VkDescriptorSet Allocate(VkDevice device, VkDescriptorSetLayout layout) const;
    };
#pragma endregion Descriptor Pool
}
