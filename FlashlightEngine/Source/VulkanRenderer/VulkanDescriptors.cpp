/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : VulkanDescriptors.cpp
 * Description : Definitions of methods from VulkanDescriptors.hpp.
 */
#include <magic_enum.hpp>
#include <FlashlightEngine/VulkanRenderer/VulkanDescriptors.hpp>

namespace Flashlight::VulkanRenderer {
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
}
