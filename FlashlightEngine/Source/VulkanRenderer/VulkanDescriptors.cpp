/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : VulkanDescriptors.cpp
 * Description : Definitions of methods from VulkanDescriptors.hpp.
 */
#include <magic_enum.hpp>
#include <FlashlightEngine/VulkanRenderer/VulkanDescriptors.hpp>

namespace Flashlight::Renderer {
#pragma region Descriptor Layout Builder
    void DescriptorLayoutBuilder::AddBinding(const u32 binding, const VkDescriptorType type) {
        VkDescriptorSetLayoutBinding newBinding{};
        newBinding.binding = binding;
        newBinding.descriptorCount = 1;
        newBinding.descriptorType = type;

        Bindings.push_back(newBinding);
    }

    void DescriptorLayoutBuilder::Clear() {
        Bindings.clear();
    }

    VkDescriptorSetLayout DescriptorLayoutBuilder::Build(const VkDevice device,
                                                         const VkShaderStageFlags shaderStages, const void* pNext,
                                                         const VkDescriptorSetLayoutCreateFlags flags) {
        for (auto& b : Bindings) {
            b.stageFlags |= shaderStages;
        }

        VkDescriptorSetLayoutCreateInfo info = {.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO};
        info.pNext = pNext;

        info.pBindings = Bindings.data();
        info.bindingCount = static_cast<u32>(Bindings.size());
        info.flags = flags;

        VkDescriptorSetLayout layout;
        VK_CHECK(vkCreateDescriptorSetLayout(device, &info, nullptr, &layout))

        return layout;
    }
#pragma endregion Descriptor Layout Builder

#pragma region Descriptor Pool
    void DescriptorAllocator::InitPool(const VkDevice device, const u32 maxSets,
                                       const std::span<PoolSizeRatio> poolRatios) {
        std::vector<VkDescriptorPoolSize> poolSizes;
        for (const PoolSizeRatio ratio : poolRatios) {
            poolSizes.push_back(VkDescriptorPoolSize{
                .type = ratio.Type, .descriptorCount = static_cast<u32>(ratio.Ratio) * maxSets
            });

            VkDescriptorPoolCreateInfo poolInfo = {.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO};
            poolInfo.flags = 0;
            poolInfo.maxSets = maxSets;
            poolInfo.poolSizeCount = static_cast<u32>(poolSizes.size());
            poolInfo.pPoolSizes = poolSizes.data();

            VK_CHECK(vkCreateDescriptorPool(device, &poolInfo, nullptr, &Pool))
        }
    }

    void DescriptorAllocator::ClearDescriptors(const VkDevice device) const {
        vkResetDescriptorPool(device, Pool, 0);
    }

    void DescriptorAllocator::DestroyPool(const VkDevice device) const {
        vkDestroyDescriptorPool(device, Pool, nullptr);
    }

    VkDescriptorSet DescriptorAllocator::Allocate(const VkDevice device, const VkDescriptorSetLayout layout) const {
        VkDescriptorSetAllocateInfo allocateInfo = {.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO};
        allocateInfo.pNext = nullptr;
        allocateInfo.descriptorPool = Pool;
        allocateInfo.descriptorSetCount = 1;
        allocateInfo.pSetLayouts = &layout;

        VkDescriptorSet set;
        VK_CHECK(vkAllocateDescriptorSets(device, &allocateInfo, &set))

        return set;
    }
#pragma endregion Descriptor Pool

#pragma region Growable Descriptor Allocator
    void DescriptorAllocatorGrowable::Initialize(const VkDevice device, const u32 initialSets,
                                                 const std::span<PoolSizeRatio> poolSizes) {
        m_Sizes.clear();

        for (auto size : poolSizes) {
            m_Sizes.push_back(size);
        }

        const VkDescriptorPool newPool = CreatePool(device, initialSets, poolSizes);

        m_SetsPerPool = static_cast<u32>(static_cast<f64>(initialSets) * 1.5);

        m_ReadyPools.push_back(newPool);
    }

    void DescriptorAllocatorGrowable::ClearPools(const VkDevice device) {
        for (const auto pool : m_ReadyPools) {
            vkResetDescriptorPool(device, pool, 0);
        }
        for (const auto pool : m_FullPools) {
            vkResetDescriptorPool(device, pool, 0);
            m_ReadyPools.push_back(pool);
        }
        m_FullPools.clear();
    }

    void DescriptorAllocatorGrowable::DestroyPools(VkDevice device) {
        for (const auto pool : m_ReadyPools) {
            vkDestroyDescriptorPool(device, pool, nullptr);
        }
        m_ReadyPools.clear();

        for (const auto pool : m_FullPools) {
            vkDestroyDescriptorPool(device, pool, nullptr);
        }
        m_FullPools.clear();
    }

    VkDescriptorSet DescriptorAllocatorGrowable::Allocate(const VkDevice device, const VkDescriptorSetLayout layout,
                                                          const void* pNext) {
        // Get or create a pool to allocate from.
        VkDescriptorPool poolToUse = GetPool(device);

        VkDescriptorSetAllocateInfo allocateInfo;
        allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocateInfo.pNext = pNext;
        allocateInfo.descriptorPool = poolToUse;
        allocateInfo.descriptorSetCount = 1;
        allocateInfo.pSetLayouts = &layout;

        VkDescriptorSet set;

        // Allocation failed. Try again.
        if (const VkResult result = vkAllocateDescriptorSets(device, &allocateInfo, &set);
            result == VK_ERROR_OUT_OF_POOL_MEMORY || result == VK_ERROR_FRAGMENTED_POOL) {
            m_FullPools.push_back(poolToUse);

            poolToUse = GetPool(device);
            allocateInfo.descriptorPool = poolToUse;

            VK_CHECK(vkAllocateDescriptorSets(device, &allocateInfo, &set))
        }

        m_ReadyPools.push_back(poolToUse);
        return set;
    }

    VkDescriptorPool DescriptorAllocatorGrowable::GetPool(const VkDevice device) {
        VkDescriptorPool newPool;
        if (!m_ReadyPools.empty()) {
            newPool = m_ReadyPools.back();
            m_ReadyPools.pop_back();
        } else {
            // Need to create a new pool.
            newPool = CreatePool(device, m_SetsPerPool, m_Sizes);

            m_SetsPerPool = static_cast<u32>(static_cast<f64>(m_SetsPerPool) * 1.5);
            if (m_SetsPerPool > 4092) {
                m_SetsPerPool = 4092;
            }
        }

        return newPool;
    }

    VkDescriptorPool DescriptorAllocatorGrowable::CreatePool(const VkDevice device, const u32 setCount,
                                                             const std::span<PoolSizeRatio> poolSizes) {
        std::vector<VkDescriptorPoolSize> sizes;
        for (const PoolSizeRatio ratio : poolSizes) {
            sizes.push_back(VkDescriptorPoolSize{
                .type = ratio.Type,
                .descriptorCount = static_cast<u32>(ratio.Ratio * static_cast<f32>(setCount))
            });
        }

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.flags = 0;
        poolInfo.maxSets = setCount;
        poolInfo.poolSizeCount = static_cast<u32>(sizes.size());
        poolInfo.pPoolSizes = sizes.data();

        VkDescriptorPool newPool;
        vkCreateDescriptorPool(device, &poolInfo, nullptr, &newPool);

        return newPool;
    }
#pragma endregion Growable Descriptor Allocator

#pragma region Descriptor Writer
    void DescriptorWriter::WriteImage(const u32 binding, const VkImageView imageView, const VkSampler sampler,
                                      const VkImageLayout layout, const VkDescriptorType type) {
        const VkDescriptorImageInfo& info = ImageInfos.emplace_back(VkDescriptorImageInfo{
            .sampler = sampler,
            .imageView = imageView,
            .imageLayout = layout
        });

        VkWriteDescriptorSet write = {.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
        write.dstBinding = binding;
        write.dstSet = VK_NULL_HANDLE; // Left empty until we need to write it.
        write.descriptorCount = 1;
        write.descriptorType = type;
        write.pImageInfo = &info;

        Writes.push_back(write);
    }

    void DescriptorWriter::WriteBuffer(const u32 binding, const VkBuffer buffer, const VkDeviceSize size,
                                       const VkDeviceSize offset, const VkDescriptorType type) {
        const VkDescriptorBufferInfo& info = BufferInfos.emplace_back(VkDescriptorBufferInfo{
            .buffer = buffer,
            .offset = offset,
            .range = size,
        });

        VkWriteDescriptorSet write = {.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
        write.dstBinding = binding;
        write.dstSet = VK_NULL_HANDLE; // Left empty until we need to write it.
        write.descriptorCount = 1;
        write.descriptorType = type;
        write.pBufferInfo = &info;

        Writes.push_back(write);
    }

    void DescriptorWriter::Clear() {
        ImageInfos.clear();
        Writes.clear();
        BufferInfos.clear();
    }

    void DescriptorWriter::UpdateSet(VkDevice device, VkDescriptorSet set) {
        for (VkWriteDescriptorSet& write : Writes) {
            write.dstSet = set;
        }

        vkUpdateDescriptorSets(device, static_cast<u32>(Writes.size()), Writes.data(), 0, nullptr);
    }
#pragma endregion Descriptor Writer
}
