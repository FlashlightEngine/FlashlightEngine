#include "FlashlightEngine/Renderer/VulkanWrapper/DescriptorSets.hpp"

namespace Flashlight::VulkanWrapper {
    /*
     * -------------------------------
     * Descriptor Set Layout & Builder
     * -------------------------------
     */
#pragma region Descriptor Set Layout
    DescriptorSetLayout::Builder::Builder(const std::shared_ptr<Device>& device) : m_BuilderDevice(device) {
    }

    DescriptorSetLayout::Builder& DescriptorSetLayout::Builder::AddBinding(const DescriptorSetBinding& binding) {
        m_Bindings.push_back(binding);

        return *this;
    }

    std::unique_ptr<DescriptorSetLayout> DescriptorSetLayout::Builder::Build() {
        return std::make_unique<DescriptorSetLayout>(m_BuilderDevice, m_Bindings);
    }

    DescriptorSetLayout::DescriptorSetLayout(const std::shared_ptr<Device>& device,
                                             const std::vector<DescriptorSetBinding>& bindings) : m_Device(device) {
        std::vector<VkDescriptorSetLayoutBinding> nativeBindings;

        for (const auto binding : bindings) {
            VkDescriptorSetLayoutBinding nativeBinding;
            nativeBinding.binding = binding.Binding;
            nativeBinding.descriptorType = binding.Type;
            nativeBinding.descriptorCount = binding.DescriptorCount;
            nativeBinding.stageFlags = binding.Stages;
            nativeBinding.pImmutableSamplers = nullptr;
            nativeBindings.push_back(nativeBinding);
        }

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = static_cast<u32>(nativeBindings.size());
        layoutInfo.pBindings = nativeBindings.data();

        if (vkCreateDescriptorSetLayout(m_Device->GetNativeDevice(), &layoutInfo, nullptr, &m_Layout) !=
            VK_SUCCESS) {
            Log::EngineFatal({0x02, 0x11}, "Failed to create descriptor set layout.");
        }
    }

    DescriptorSetLayout::~DescriptorSetLayout() {
        if (m_Layout) {
            vkDestroyDescriptorSetLayout(m_Device->GetNativeDevice(), m_Layout, nullptr);
        }
    }

    DescriptorSetLayout::DescriptorSetLayout(DescriptorSetLayout&& rhs) noexcept {
        m_Layout = rhs.m_Layout;
        m_Device = rhs.m_Device;

        rhs.m_Layout = VK_NULL_HANDLE;
        rhs.m_Device = nullptr;
    }

    DescriptorSetLayout& DescriptorSetLayout::operator=(DescriptorSetLayout&& rhs) noexcept {
        m_Layout = rhs.m_Layout;
        m_Device = rhs.m_Device;

        rhs.m_Layout = VK_NULL_HANDLE;
        rhs.m_Device = nullptr;

        return *this;
    }
#pragma endregion Descriptor Set Layout

    /*
     * -------------------------
     * Descriptor Pool & Builder
     * -------------------------
     */
#pragma region Descriptor Pool
    DescriptorPool::Builder::Builder(const std::shared_ptr<Device>& device) : m_BuilderDevice(device) {
    }

    DescriptorPool::Builder& DescriptorPool::Builder::AddSize(const VkDescriptorPoolSize& size) {
        m_Sizes.push_back(size);

        return *this;
    }

    std::unique_ptr<DescriptorPool> DescriptorPool::Builder::Build(const u32 maxSetsNumber) {
        return std::make_unique<DescriptorPool>(m_BuilderDevice, m_Sizes, maxSetsNumber);
    }

    DescriptorPool::DescriptorPool(const std::shared_ptr<Device>& device,
                                   const std::vector<VkDescriptorPoolSize>& sizes,
                                   const u32 maxSetsNumber) : m_Device(device) {
        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<u32>(sizes.size());
        poolInfo.pPoolSizes = sizes.data();
        poolInfo.maxSets = maxSetsNumber;

        if (vkCreateDescriptorPool(m_Device->GetNativeDevice(), &poolInfo, nullptr, &m_Pool) != VK_SUCCESS) {
            Log::EngineFatal({0x02, 0x12}, "Failed to create descriptor pool.");
        }
    }

    DescriptorPool::~DescriptorPool() {
        if (m_Pool) {
            vkDestroyDescriptorPool(m_Device->GetNativeDevice(), m_Pool, nullptr);
        }
    }

    DescriptorPool::DescriptorPool(DescriptorPool&& rhs) noexcept {
        m_Pool = rhs.m_Pool;
        m_Device = rhs.m_Device;

        rhs.m_Pool = VK_NULL_HANDLE;
        rhs.m_Device = nullptr;
    }

    DescriptorPool& DescriptorPool::operator=(DescriptorPool&& rhs) noexcept {
        m_Pool = rhs.m_Pool;
        m_Device = rhs.m_Device;

        rhs.m_Pool = VK_NULL_HANDLE;
        rhs.m_Device = nullptr;

        return *this;
    }

    std::vector<VkDescriptorSet> DescriptorPool::AllocateDescriptorSets(
        const u32 count, const std::vector<VkDescriptorSetLayout>& layouts) const {
        std::vector<VkDescriptorSet> descriptorSets(count);

        VkDescriptorSetAllocateInfo allocateInfo{};
        allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocateInfo.descriptorPool = m_Pool;
        allocateInfo.descriptorSetCount = count;
        allocateInfo.pSetLayouts = layouts.data();

        if (vkAllocateDescriptorSets(m_Device->GetNativeDevice(), &allocateInfo, descriptorSets.data())
            != VK_SUCCESS) {
            Log::EngineFatal({0x02, 0x13}, "Failed to allocate descriptor set(s).");
        }

        return descriptorSets;
    }
#pragma endregion Descriptor Pool
}
