#include "FlashlightEngine/Renderer/VulkanWrapper/DescriptorSet.hpp"

namespace Flashlight::VulkanWrapper {
    DescriptorSetLayout::DescriptorSetLayout(const Device& device, const VkPipelineStageFlags stages) : m_Device(device) {
        m_UboLayoutBinding.binding = 0;
        m_UboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        m_UboLayoutBinding.descriptorCount = 1;
        m_UboLayoutBinding.stageFlags = stages;
        m_UboLayoutBinding.pImmutableSamplers = VK_NULL_HANDLE;

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = 1;
        layoutInfo.pBindings = &m_UboLayoutBinding;

        Log::EngineTrace("Creating Vulkan descriptor set layout...");
        if (vkCreateDescriptorSetLayout(m_Device.GetNativeDevice(), &layoutInfo, nullptr, &m_Layout)
            != VK_SUCCESS) {
            Log::EngineError("Failed to create Vulkan descriptor set layout.");
        } else {
            Log::EngineTrace("Vulkan descriptor set layout created.");
        }
    }

    DescriptorSetLayout::~DescriptorSetLayout() {
        if (m_Layout) {
            Log::EngineTrace("Destroying Vulkan descriptor set layout.");
            vkDestroyDescriptorSetLayout(m_Device.GetNativeDevice(), m_Layout, nullptr);
        }
    }

}
