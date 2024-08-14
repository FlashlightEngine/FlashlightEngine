#pragma once

#include <FlashlightEngine/VulkanRenderer/VulkanTypes.hpp>
#include <FlashlightEngine/VulkanRenderer/VulkanDescriptors.hpp>
#include <FlashlightEngine/VulkanRenderer/VulkanUtils/VulkanImageUtils.hpp>

namespace Flashlight::Renderer {
    enum class MaterialPass : u8 {
        MainColor,
        Transparent,
        Other
    };
        
    struct MaterialPipeline {
        VkPipeline Pipeline;
        VkPipelineLayout PipelineLayout;
    };

    struct MaterialInstance {
        MaterialPipeline* Pipeline;
        VkDescriptorSet MaterialSet;
        MaterialPass PassType;
    };

    struct GLTFMetallic_Roughness {
        MaterialPipeline OpaquePipeline;
        MaterialPipeline TransparentPipeline;

        VkDescriptorSetLayout MaterialLayout;

        struct MaterialConstants {
            glm::vec4 ColorFactors;
            glm::vec4 MetalRoughFactors;
            // Padding, we need it anyway for uniform buffers.
            glm::vec4 Padding[14];
        };

        struct MaterialResources {
            AllocatedImage ColorImage;
            VkSampler ColorSampler;
            AllocatedImage MetalRoughnessImage;
            VkSampler MetalRoughnessSampler;
            VkBuffer DataBuffer;
            u32 DataBufferOffset;
        };

        DescriptorWriter Writer;

        void BuildPipelines(VulkanRenderer* renderer);
        void ClearResources(VkDevice device);

        MaterialInstance WriteMaterial(VkDevice device, MaterialPass pass, const MaterialResources& resources,
                                       DescriptorAllocatorGrowable& descriptorAllocator);
    };
}