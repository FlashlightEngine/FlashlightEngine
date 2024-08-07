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

namespace Flashlight::Renderer {
#pragma region Descriptor Layout Builder
    struct DescriptorLayoutBuilder {
        std::vector<VkDescriptorSetLayoutBinding> Bindings;

        void AddBinding(u32 binding, VkDescriptorType type);
        void Clear();
        [[nodiscard]] VkDescriptorSetLayout Build(VkDevice device, VkShaderStageFlags shaderStages,
                                                  const void* pNext = nullptr,
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

        [[nodiscard]] VkDescriptorSet Allocate(VkDevice device, VkDescriptorSetLayout layout) const;
    };
#pragma endregion Descriptor Pool

#pragma region Growable Descriptor Allocator
    struct DescriptorAllocatorGrowable {
        struct PoolSizeRatio {
            VkDescriptorType Type;
            f32 Ratio;
        };

        void Initialize(VkDevice device, u32 initialSets, std::span<PoolSizeRatio> poolSizes);
        void ClearPools(VkDevice device);
        void DestroyPools(VkDevice device);

        [[nodiscard]] VkDescriptorSet Allocate(VkDevice device, VkDescriptorSetLayout layout,
                                               const void* pNext = nullptr);

    private:
        [[nodiscard]] VkDescriptorPool GetPool(VkDevice device);
        [[nodiscard]] static VkDescriptorPool CreatePool(VkDevice device, u32 setCount,
                                                         std::span<PoolSizeRatio> poolSizes);

        std::vector<PoolSizeRatio> m_Sizes;
        std::vector<VkDescriptorPool> m_FullPools;
        std::vector<VkDescriptorPool> m_ReadyPools;
        u32 m_SetsPerPool{0};
    };
#pragma endregion Growable Descriptor Allocator

#pragma region Descriptor Writer
    struct DescriptorWriter {
        std::deque<VkDescriptorImageInfo> ImageInfos;
        std::deque<VkDescriptorBufferInfo> BufferInfos;
        std::vector<VkWriteDescriptorSet> Writes;

        void WriteImage(u32 binding, VkImageView imageView, VkSampler sampler, VkImageLayout layout,
                        VkDescriptorType type);
        void WriteBuffer(u32 binding, VkBuffer buffer, VkDeviceSize size, VkDeviceSize offset,
                         VkDescriptorType type);

        void Clear();
        void UpdateSet(VkDevice device, VkDescriptorSet set);
    };
#pragma endregion Descriptor Writer
}
