#include <FlashlightEngine/Managers/SceneManager.hpp>

#include <FlashlightEngine/VulkanRenderer/VulkanInitializers.hpp>
#include <FlashlightEngine/VulkanRenderer/VulkanUtils/VulkanBufferUtils.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

namespace Flashlight {
    SceneManager::SceneManager(Renderer::VulkanRenderer* renderer) : m_LinkedRenderer(renderer) {
    }

    SceneManager::~SceneManager() {
        ClearScenes();
    }

    void SceneManager::LoadScene(const std::filesystem::path& path, const std::string& name) {
        const auto file = LoadGLTF(m_LinkedRenderer, path);

        Scene newScene;
        newScene.SceneModel = *file;

        newScene.SceneData.AmbientColor = glm::vec4(.1f);
        newScene.SceneData.SunlightColor = glm::vec4(1.f);
        newScene.SceneData.SunlightDirection = glm::vec4(0, 1, 0.5f, 1.0f);

        if (file.has_value()) {
            m_LoadedScenes[name] = newScene;
        }
    }

    void SceneManager::UnloadScene(const std::string& name) {
        vkDeviceWaitIdle(m_LinkedRenderer->GetDevice().GetDevice());

        if (const auto it = m_LoadedScenes.find(name);
            it != m_LoadedScenes.end()) {
            m_LoadedScenes.erase(it);
        }
    }

    void SceneManager::UpdateScene(const Renderer::Camera& camera, const Window& window, const std::string& name) {
        m_LoadedScenes[name].DrawContext.OpaqueSurfaces.clear();
        m_LoadedScenes[name].DrawContext.TransparentSurfaces.clear();

        m_LoadedScenes[name].SceneData.View = camera.GetViewMatrix();
        m_LoadedScenes[name].SceneData.Projection = glm::perspective(glm::radians(70.f),
                                                                     static_cast<f32>(window.GetWidth()) /
                                                                     static_cast<f32>(window.GetHeight()), 0.1f,
                                                                     10000.0f);

        m_LoadedScenes[name].SceneData.Projection[1][1] *= -1;
        m_LoadedScenes[name].SceneData.ViewProjection = m_LoadedScenes[name].SceneData.Projection * m_LoadedScenes[
            name].SceneData.View;

        m_LoadedScenes[name].SceneModel->Draw(glm::mat4{1.f}, m_LoadedScenes[name].DrawContext);
    }

    void SceneManager::RenderScene(const std::string& name, EngineStats& stats) {
        auto& frame = m_LinkedRenderer->GetCurrentFrame();

        stats.TriangleCount = 0;
        stats.DrawCallCount = 0;

        std::vector<u32> opaqueDraws;
        opaqueDraws.reserve(m_LoadedScenes[name].DrawContext.OpaqueSurfaces.size());

        for (u32 i = 0; i < m_LoadedScenes[name].DrawContext.OpaqueSurfaces.size(); i++) {
            if (Renderer::VulkanRenderer::IsVisible(m_LoadedScenes[name].DrawContext.OpaqueSurfaces[i],
                                                    m_LoadedScenes[name].SceneData.ViewProjection)) {
                opaqueDraws.push_back(i);
            }
        }

        // Sort the opaques surfaces by material and mesh.
        std::ranges::sort(opaqueDraws, [&](const auto& iA, const auto& iB) {
            const Renderer::RenderObject& A = m_LoadedScenes[name].DrawContext.OpaqueSurfaces[iA];
            const Renderer::RenderObject& B = m_LoadedScenes[name].DrawContext.OpaqueSurfaces[iB];
            if (A.Material == B.Material) {
                return A.IndexBuffer < B.IndexBuffer;
            }

            return A.Material < B.Material;
        });

        const auto start = std::chrono::system_clock::now();

        // Begin a render pass connected to the draw image.
        const VkRenderingAttachmentInfo colorAttachment = Renderer::VulkanInit::AttachmentInfo(
            m_LinkedRenderer->DrawImage.ImageView, nullptr, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
        const VkRenderingAttachmentInfo depthAttachment = Renderer::VulkanInit::DepthAttachmentInfo(
            m_LinkedRenderer->DepthImage.ImageView, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);

        const VkRenderingInfo renderingInfo = Renderer::VulkanInit::RenderingInfo(
            m_LinkedRenderer->DrawExtent, &colorAttachment, &depthAttachment);
        vkCmdBeginRendering(frame.MainCommandBuffer, &renderingInfo);

        void* mappedMemory = nullptr;

        Renderer::VulkanUtils::AllocatedBuffer gpuSceneDataBuffer = Renderer::VulkanUtils::CreateBuffer(
            m_LinkedRenderer->GetAllocator(), sizeof(Renderer::GPUSceneData),
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, Renderer::VulkanUtils::MemoryUsage::CpuToGpu);

        // Add it to the deletion queue of this frame so that it gets deleted once it's been used.
        frame.DeletionQueue.PushFunction([this, gpuSceneDataBuffer]() {
            Renderer::VulkanUtils::DestroyBuffer(m_LinkedRenderer->GetAllocator(), gpuSceneDataBuffer);
        });

        // Write the buffer.
        auto sceneUniformData = static_cast<Renderer::GPUSceneData*>(gpuSceneDataBuffer.Allocation->
                                                                                        GetMappedData());
        *sceneUniformData = m_LoadedScenes[name].SceneData;

        // Create a descriptor set that binds that buffer and update it.
        VkDescriptorSet globalDescriptor = frame.FrameDescriptors.Allocate(
            m_LinkedRenderer->GetDevice().GetDevice(), m_LinkedRenderer->GpuSceneDataLayout);
        {
            Renderer::DescriptorWriter writer;
            writer.WriteBuffer(0, gpuSceneDataBuffer.Buffer, sizeof(Renderer::GPUSceneData), 0,
                               VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
            writer.UpdateSet(m_LinkedRenderer->GetDevice().GetDevice(), globalDescriptor);
        }

        Renderer::MaterialPipeline* lastPipeline = nullptr;
        Renderer::MaterialInstance* lastMaterial = nullptr;
        VkBuffer lastIndexBuffer = VK_NULL_HANDLE;

        auto draw = [&](const Renderer::RenderObject& drawnObject) {
            if (drawnObject.Material != lastMaterial) {
                lastMaterial = drawnObject.Material;

                // Rebind pipeline and descriptors if the material changed.
                if (drawnObject.Material->Pipeline != lastPipeline) {
                    lastPipeline = drawnObject.Material->Pipeline;
                    vkCmdBindPipeline(frame.MainCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                      drawnObject.Material->Pipeline->Pipeline);
                    vkCmdBindDescriptorSets(frame.MainCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                            drawnObject.Material->Pipeline->PipelineLayout, 0, 1, &globalDescriptor,
                                            0, nullptr);

                    VkViewport viewport;
                    viewport.x = 0;
                    viewport.y = 0;
                    viewport.width = static_cast<f32>(m_LinkedRenderer->DrawExtent.width);
                    viewport.height = static_cast<f32>(m_LinkedRenderer->DrawExtent.height);
                    viewport.minDepth = 0.f;
                    viewport.maxDepth = 1.f;

                    vkCmdSetViewport(frame.MainCommandBuffer, 0, 1, &viewport);

                    VkRect2D scissor;
                    scissor.offset.x = 0;
                    scissor.offset.y = 0;
                    scissor.extent.width = m_LinkedRenderer->DrawExtent.width;
                    scissor.extent.height = m_LinkedRenderer->DrawExtent.height;

                    vkCmdSetScissor(frame.MainCommandBuffer, 0, 1, &scissor);
                }

                vkCmdBindDescriptorSets(frame.MainCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                        drawnObject.Material->Pipeline->PipelineLayout, 1, 1,
                                        &drawnObject.Material->MaterialSet, 0, nullptr);
            }

            // Rebind index buffer if needed.
            if (drawnObject.IndexBuffer != lastIndexBuffer) {
                lastIndexBuffer = drawnObject.IndexBuffer;
                vkCmdBindIndexBuffer(frame.MainCommandBuffer, drawnObject.IndexBuffer, 0, VK_INDEX_TYPE_UINT32);
            }

            // Calculate final mesh matrices.
            Renderer::GPUDrawPushConstants pushConstants;
            pushConstants.WorldMatrix = drawnObject.Transform;
            pushConstants.VertexBuffer = drawnObject.VertexBufferAddress;

            vkCmdPushConstants(frame.MainCommandBuffer, drawnObject.Material->Pipeline->PipelineLayout,
                               VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(Renderer::GPUDrawPushConstants),
                               &pushConstants);

            vkCmdDrawIndexed(frame.MainCommandBuffer, drawnObject.IndexCount, 1, drawnObject.FirstIndex, 0, 0);

            stats.DrawCallCount++;
            stats.TriangleCount += static_cast<i32>(drawnObject.IndexCount) / 3;
        };

        for (auto& objectIndex : opaqueDraws) {
            draw(m_LoadedScenes[name].DrawContext.OpaqueSurfaces[objectIndex]);
        }

        for (const Renderer::RenderObject& object : m_LoadedScenes[name].DrawContext.
                                                                         TransparentSurfaces) {
            draw(object);
        }

        vkCmdEndRendering(frame.MainCommandBuffer);

        m_LoadedScenes[name].DrawContext.OpaqueSurfaces.clear();
        m_LoadedScenes[name].DrawContext.TransparentSurfaces.clear();

        const auto end = std::chrono::system_clock::now();

        const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        stats.MeshDrawTime = static_cast<f32>(elapsed.count()) / 1000.f;
    }

    void SceneManager::ClearScenes() {
        vkDeviceWaitIdle(m_LinkedRenderer->GetDevice().GetDevice());
        m_LoadedScenes.clear();
    }
}
