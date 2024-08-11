#include <FlashlightEngine/VulkanRenderer/MaterialSystem.hpp>

#include <FlashlightEngine/VulkanRenderer/VulkanInitializers.hpp>
#include <FlashlightEngine/VulkanRenderer/VulkanRenderer.hpp>
#include <FlashlightEngine/VulkanRenderer/VulkanUtils/VulkanPipelineUtils.hpp>

namespace Flashlight::Renderer {
    void GLTFMetallic_Roughness::BuildPipelines(VulkanRenderer* renderer) {
        VkShaderModule meshFragShader;
        if (!VulkanUtils::CreateShaderModule(renderer->GetDevice().GetDevice(), "Shaders/mesh.frag.spv",
                                             &meshFragShader)) {
            Log::EngineError("Failed to load mesh fragment shader.");
                                             }

        VkShaderModule meshVertexShader;
        if (!VulkanUtils::CreateShaderModule(renderer->GetDevice().GetDevice(), "Shaders/mesh.vert.spv",
                                             &meshVertexShader)) {
            Log::EngineError("Failed to load mesh vertex shader.");
                                             }

        VkPushConstantRange matrixRange;
        matrixRange.offset = 0;
        matrixRange.size = sizeof(GPUDrawPushConstants);
        matrixRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        DescriptorLayoutBuilder layoutBuilder;
        layoutBuilder.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
        layoutBuilder.AddBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
        layoutBuilder.AddBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);

        MaterialLayout = layoutBuilder.Build(renderer->GetDevice().GetDevice(),
                                             VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);

        renderer->PlanDeletion([this, renderer]() {
            vkDestroyDescriptorSetLayout(renderer->GetDevice().GetDevice(), MaterialLayout, nullptr);
        });

        const VkDescriptorSetLayout layouts[] = {renderer->GpuSceneDataLayout, MaterialLayout};

        VkPipelineLayoutCreateInfo meshLayoutInfo = VulkanInit::PipelineLayoutCreateInfo();
        meshLayoutInfo.setLayoutCount = 2;
        meshLayoutInfo.pSetLayouts = layouts;
        meshLayoutInfo.pushConstantRangeCount = 1;
        meshLayoutInfo.pPushConstantRanges = &matrixRange;

        VkPipelineLayout newLayout;
        VK_CHECK(vkCreatePipelineLayout(renderer->GetDevice().GetDevice(), &meshLayoutInfo, nullptr, &newLayout))

        OpaquePipeline.PipelineLayout = newLayout;
        TransparentPipeline.PipelineLayout = newLayout;

        // Configure the pipelines.
        VulkanUtils::PipelineBuilder pipelineBuilder;
        pipelineBuilder.SetShaders(meshVertexShader, meshFragShader);
        pipelineBuilder.SetInputTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
        pipelineBuilder.SetPolygonMode(VK_POLYGON_MODE_FILL);
        pipelineBuilder.SetCullMode(VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE);
        pipelineBuilder.SetMultisamplingNone();
        pipelineBuilder.DisableBlending();
        pipelineBuilder.EnableDepthTest(true, VK_COMPARE_OP_LESS);

        // Render format
        pipelineBuilder.SetColorAttachmentFormat(renderer->GetDrawImageFormat());
        pipelineBuilder.SetDepthFormat(renderer->GetDepthImageFormat());

        pipelineBuilder.SetPipelineLayout(newLayout);

        // Build pipeline
        OpaquePipeline.Pipeline = pipelineBuilder.BuildPipeline(renderer->GetDevice().GetDevice());

        // Create the transparent variant.
        pipelineBuilder.EnableAdditiveBlending();

        pipelineBuilder.SetCullMode(VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE);
        pipelineBuilder.EnableDepthTest(false, VK_COMPARE_OP_LESS);

        TransparentPipeline.Pipeline = pipelineBuilder.BuildPipeline(renderer->GetDevice().GetDevice());

        vkDestroyShaderModule(renderer->GetDevice().GetDevice(), meshFragShader, nullptr);
        vkDestroyShaderModule(renderer->GetDevice().GetDevice(), meshVertexShader, nullptr);

        renderer->PlanDeletion([this, renderer, newLayout]() {
            vkDestroyPipelineLayout(renderer->GetDevice().GetDevice(), newLayout, nullptr);
            vkDestroyPipeline(renderer->GetDevice().GetDevice(), OpaquePipeline.Pipeline, nullptr);
            vkDestroyPipeline(renderer->GetDevice().GetDevice(), TransparentPipeline.Pipeline, nullptr);
        });
    }

    void GLTFMetallic_Roughness::ClearResources(VkDevice device) {
    }

    MaterialInstance GLTFMetallic_Roughness::WriteMaterial(const VkDevice device, const MaterialPass pass,
                                                           const MaterialResources& resources,
                                                           DescriptorAllocatorGrowable& descriptorAllocator) {
        MaterialInstance matData;
        matData.PassType = pass;

        matData.Pipeline = &OpaquePipeline;
        if (pass == MaterialPass::Transparent) {
            matData.Pipeline = &TransparentPipeline;
        }

        matData.MaterialSet = descriptorAllocator.Allocate(device, MaterialLayout);

        Writer.Clear();

        Writer.WriteBuffer(0, resources.DataBuffer, sizeof(MaterialConstants), resources.DataBufferOffset,
                           VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
        Writer.WriteImage(1, resources.ColorImage.ImageView, resources.ColorSampler,
                          VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
        Writer.WriteImage(2, resources.MetalRoughnessImage.ImageView, resources.MetalRoughnessSampler,
                          VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);

        Writer.UpdateSet(device, matData.MaterialSet);

        return matData;
    }
}