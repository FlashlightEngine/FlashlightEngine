#include "FlashlightEngine/Renderer/VulkanWrapper/DescriptorSet.hpp"

namespace Flashlight::VulkanWrapper {
    /*
     * Descriptor set layout
     */
    DescriptorSetLayout::DescriptorSetLayout(const Device& device,
                                             const std::vector<VkDescriptorSetLayoutBinding>& bindings)
    : m_Device(device) {
        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = static_cast<u32>(bindings.size());
        layoutInfo.pBindings = bindings.data();

        if (vkCreateDescriptorSetLayout(m_Device.GetNativeDevice(), &layoutInfo, nullptr, &m_Layout)
            != VK_SUCCESS) {
            Log::EngineError("Failed to create Vulkan descriptor set layout.");
        }
    }

    DescriptorSetLayout::~DescriptorSetLayout() {
        if (m_Layout) {
            vkDestroyDescriptorSetLayout(m_Device.GetNativeDevice(), m_Layout, nullptr);
        }
    }

    DescriptorSetLayout::DescriptorSetLayout(DescriptorSetLayout&& rhs) noexcept : m_Device(rhs.m_Device) {
        m_Layout = rhs.m_Layout;

        rhs.m_Layout = VK_NULL_HANDLE;
    }

    DescriptorSetLayout& DescriptorSetLayout::operator=(DescriptorSetLayout&& rhs) noexcept {
        m_Layout = rhs.m_Layout;

        rhs.m_Layout = VK_NULL_HANDLE;

        return *this;
    }

    /*
     * Descriptor set layout builder
     */
    DescriptorSetLayout::Builder& DescriptorSetLayout::Builder::AddEntry(const DescriptorSetLayoutEntry& entry) {
        m_Entries.push_back(entry);

        return *this;
    }

    std::unique_ptr<DescriptorSetLayout> DescriptorSetLayout::Builder::Build(const Device& device) const {
        std::vector<VkDescriptorSetLayoutBinding> bindings(m_Entries.size());
        u32 i = 0;
        for (const auto& entry : m_Entries) {
            VkDescriptorSetLayoutBinding binding{};
            binding.binding = entry.BindingNumber;
            binding.descriptorType = entry.DescriptorType;
            binding.descriptorCount = entry.BindingCount;
            binding.stageFlags = entry.Stages;
            binding.pImmutableSamplers = nullptr;
            bindings[i] = binding;
            
            i++;
        }

        return std::make_unique<DescriptorSetLayout>(device, bindings);
    }

}
