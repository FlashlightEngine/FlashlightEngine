#pragma once

#include "FlashlightEngine/Renderer/VulkanWrapper/Device.hpp"

namespace Flashlight::VulkanWrapper {
#pragma region Descriptor Set Layout
    struct DescriptorSetBinding {
        u32 Binding;
        VkDescriptorType Type;
        u32 DescriptorCount;
        VkPipelineStageFlags Stages;
    };
    
    class DescriptorSetLayout {
        VkDescriptorSetLayout m_Layout = VK_NULL_HANDLE;
        
        std::shared_ptr<Device> m_Device;

    public:
        class Builder {
            std::vector<DescriptorSetBinding> m_Bindings;

            std::shared_ptr<Device> m_BuilderDevice;
            
        public:
            explicit Builder(const std::shared_ptr<Device>& device);

            Builder& AddBinding(const DescriptorSetBinding& binding);
            
            std::unique_ptr<DescriptorSetLayout> Build();
        };

        DescriptorSetLayout(const std::shared_ptr<Device>& device, const std::vector<DescriptorSetBinding>& bindings);
        ~DescriptorSetLayout();

        DescriptorSetLayout(const DescriptorSetLayout&) = delete;
        DescriptorSetLayout(DescriptorSetLayout&& rhs) noexcept;

        DescriptorSetLayout& operator=(const DescriptorSetLayout&) = delete;
        DescriptorSetLayout& operator=(DescriptorSetLayout&& rhs) noexcept;
        
        [[nodiscard]] inline VkDescriptorSetLayout GetNativeLayout() const;
    };
#pragma endregion Descriptor Set Layout

#pragma region Descriptor Pool
    class DescriptorPool {
        VkDescriptorPool m_Pool = VK_NULL_HANDLE;

        std::shared_ptr<Device> m_Device;

    public:
        class Builder {
            std::vector<VkDescriptorPoolSize> m_Sizes;
            
            std::shared_ptr<Device> m_BuilderDevice;

        public:
            explicit Builder(const std::shared_ptr<Device>& device);

            Builder& AddSize(const VkDescriptorPoolSize& size);

            std::unique_ptr<DescriptorPool> Build(u32 maxSetsNumber);
        };

        DescriptorPool(const std::shared_ptr<Device>& device, const std::vector<VkDescriptorPoolSize>& sizes,
                       u32 maxSetsNumber);
        ~DescriptorPool();

        DescriptorPool(const DescriptorPool&) = delete;
        DescriptorPool(DescriptorPool&& rhs) noexcept;

        DescriptorPool& operator=(const DescriptorPool&) = delete;
        DescriptorPool& operator=(DescriptorPool&& rhs) noexcept;

        [[nodiscard]] inline VkDescriptorPool GetNativePool() const;

        std::vector<VkDescriptorSet> AllocateDescriptorSets(u32 count, const std::vector<VkDescriptorSetLayout>& layouts) const;
    };
#pragma endregion Descriptor Pool

#include "DescriptorSets.inl"
}
