/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : VulkanRenderer.cpp
 * Description : Definitions of methods from the VulkanRenderer class.
 */
#include <FlashlightEngine/VulkanRenderer/VulkanRenderer.hpp>

#include <FlashlightEngine/VulkanRenderer/VulkanInitializers.hpp>
#include <FlashlightEngine/VulkanRenderer/VulkanUtils/VulkanPipelineUtils.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include <vk_mem_alloc.h>

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

    void MeshNode::Draw(const glm::mat4& topMatrix, DrawContext& context) {
        glm::mat4 nodeMatrix = topMatrix * WorldTransform;

        for (auto& s : Mesh->Surfaces) {
            RenderObject def;
            def.IndexCount = s.Count;
            def.FirstIndex = s.StartIndex;
            def.IndexBuffer = Mesh->MeshBuffers.IndexBuffer.Buffer;
            def.Material = &s.Material->Data;
            def.Bounds = s.Bounds;
            def.Transform = nodeMatrix;
            def.VertexBufferAddress = Mesh->MeshBuffers.VertexBufferAddress;

            if (s.Material->Data.PassType == MaterialPass::Transparent) {
                context.TransparentSurfaces.push_back(def);
            } else {
                context.OpaqueSurfaces.push_back(def);
            }
        }

        // recurse down
        Node::Draw(topMatrix, context);
    }

    VulkanRenderer::VulkanRenderer(const Window& window, const DebugLevel& debugLevel) {
        InitializeVulkan(window, debugLevel);
        InitializeSwapchain(window);
        InitializeCommands();
        InitializeSynchronisationPrimitives();
        InitializeDescriptors();
        InitializePipelines();
        InitializeImGui(window);
        InitializeDefaultData();

        m_RendererInitialized = true;
    }

    VulkanRenderer::~VulkanRenderer() {
        if (m_RendererInitialized) {
            vkDeviceWaitIdle(m_Device->GetDevice());

            LoadedScenes.clear();

            m_MainDeletionQueue.Flush();

            m_RendererInitialized = false;
        }
    }

    GPUMeshBuffers VulkanRenderer::UploadMesh(const std::span<u32> indices,
                                              const std::span<Vertex> vertices) const {
        const size vertexBufferSize = vertices.size() * sizeof(Vertex);
        const size indexBufferSize = indices.size() * sizeof(u32);

        GPUMeshBuffers meshBuffers;

        // Create the vertex buffer
        meshBuffers.VertexBuffer = VulkanUtils::CreateBuffer(m_Allocator, vertexBufferSize,
                                                             VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
                                                             VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                                                             VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
                                                             VulkanUtils::MemoryUsage::GpuOnly);

        // Find the GPU address of the vertex buffer.
        const VkBufferDeviceAddressInfo deviceAddressInfo = {
            .sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO, .pNext = nullptr,
            .buffer = meshBuffers.VertexBuffer.Buffer
        };
        meshBuffers.VertexBufferAddress = vkGetBufferDeviceAddress(m_Device->GetDevice(), &deviceAddressInfo);

        // Create index buffer.
        meshBuffers.IndexBuffer = VulkanUtils::CreateBuffer(m_Allocator, indexBufferSize,
                                                            VK_BUFFER_USAGE_INDEX_BUFFER_BIT |
                                                            VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                                            VulkanUtils::MemoryUsage::GpuOnly);

        // Staging buffer
        const VulkanUtils::AllocatedBuffer staging = VulkanUtils::CreateBuffer(m_Allocator, vertexBufferSize + indexBufferSize,
                                                                  VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                                                  VulkanUtils::MemoryUsage::CpuOnly);

        VmaAllocationInfo stagingBufferAllocInfo;
        vmaGetAllocationInfo(m_Allocator, staging.Allocation, &stagingBufferAllocInfo);
        
        void* data = stagingBufferAllocInfo.pMappedData;

        // Copy vertex buffer.
        memcpy(data, vertices.data(), vertexBufferSize);
        // Copy index buffer.
        memcpy(static_cast<char*>(data) + vertexBufferSize, indices.data(), indexBufferSize);

        ImmediateSubmit([&](const VkCommandBuffer commandBuffer) {
            VkBufferCopy vertexCopy;
            vertexCopy.dstOffset = 0;
            vertexCopy.srcOffset = 0;
            vertexCopy.size = vertexBufferSize;

            vkCmdCopyBuffer(commandBuffer, staging.Buffer, meshBuffers.VertexBuffer.Buffer, 1, &vertexCopy);

            VkBufferCopy indexCopy;
            indexCopy.dstOffset = 0;
            indexCopy.srcOffset = vertexBufferSize;
            indexCopy.size = indexBufferSize;

            vkCmdCopyBuffer(commandBuffer, staging.Buffer, meshBuffers.IndexBuffer.Buffer, 1, &indexCopy);
        });

        DestroyBuffer(m_Allocator, staging);

        return meshBuffers;
    }

    void VulkanRenderer::PlanMeshDeletion(GPUMeshBuffers mesh) {
        m_MainDeletionQueue.PushFunction([this, mesh]() {
            DestroyBuffer(m_Allocator, mesh.IndexBuffer);
            DestroyBuffer(m_Allocator, mesh.VertexBuffer);
        });
    }

    void VulkanRenderer::PlanDescriptorPoolsDeletion(DescriptorAllocatorGrowable& allocator) {
        m_MainDeletionQueue.PushFunction([this, &allocator]() {
            allocator.ClearPools(m_Device->GetDevice());
            allocator.DestroyPools(m_Device->GetDevice());
        });
    }

    void VulkanRenderer::PlanDeletion(std::function<void()>&& deletor) {
        m_MainDeletionQueue.PushFunction(std::move(deletor));
    }

    void VulkanRenderer::BeginUi() {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();

        ImGui::NewFrame();
    }

    void VulkanRenderer::BeginRendering(const Window& window, const VkClearColorValue clearColor, EngineStats& stats) {
        ImGui::Render();

        DrawExtent.width = static_cast<u32>(static_cast<f32>(std::min(
            m_Swapchain->GetSwapchainExtent().width, DrawImage.ImageExtent.width)));
        DrawExtent.height = static_cast<u32>(static_cast<f32>(std::min(
            m_Swapchain->GetSwapchainExtent().height, DrawImage.ImageExtent.height)));

        auto& frame = GetCurrentFrame();

        const VkResult lastVkError = m_Swapchain->AcquireNextImage(*m_Device, frame);
        if (lastVkError == VK_ERROR_OUT_OF_DATE_KHR || lastVkError == VK_SUBOPTIMAL_KHR || window.
            ShouldInvalidateSwapchain()) {
            m_SwapchainResizeRequired = true;
        }

        frame.DeletionQueue.Flush();
        frame.FrameDescriptors.ClearPools(m_Device->GetDevice());

        const VkCommandBuffer cmd = frame.MainCommandBuffer;

        VK_CHECK(vkResetCommandBuffer(cmd, 0))

        const VkCommandBufferBeginInfo cmdBeginInfo = VulkanInit::CommandBufferBeginInfo(
            VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

        VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo))

        // Transition the image to the general layout so that it can be cleared.
        VulkanUtils::TransitionImage(frame.MainCommandBuffer, DrawImage.Image, VK_IMAGE_LAYOUT_UNDEFINED,
                                     VK_IMAGE_LAYOUT_GENERAL);

        const VkImageSubresourceRange clearRange = VulkanInit::ImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT);

        vkCmdClearColorImage(frame.MainCommandBuffer, DrawImage.Image, VK_IMAGE_LAYOUT_GENERAL, &clearColor, 1,
                             &clearRange);

        // Transition the draw image and the depth image to the correct layout to draw on them.
        VulkanUtils::TransitionImage(frame.MainCommandBuffer, DrawImage.Image, VK_IMAGE_LAYOUT_GENERAL,
                                     VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
        VulkanUtils::TransitionImage(frame.MainCommandBuffer, DepthImage.Image, VK_IMAGE_LAYOUT_UNDEFINED,
                                     VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);
    }

    void VulkanRenderer::EndRendering(Window& window) {
        const auto& frame = GetCurrentFrame();

        // Transition the draw image to the transfer src and the swapchain image to the transfer dst layout and copy
        // the draw image into the swapchain.
        VulkanUtils::TransitionImage(frame.MainCommandBuffer, DrawImage.Image,
                                     VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                     VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

        VulkanUtils::TransitionImage(frame.MainCommandBuffer,
                                     m_Swapchain->GetImageAtIndex(frame.SwapchainImageIndex),
                                     VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

        VulkanUtils::CopyImageToImage(frame.MainCommandBuffer, DrawImage.Image,
                                      m_Swapchain->GetImageAtIndex(frame.SwapchainImageIndex), DrawExtent,
                                      m_Swapchain->GetSwapchainExtent());

        // Transition the swapchain image to the color attachment layout so that ImGui can draw onto it.
        VulkanUtils::TransitionImage(frame.MainCommandBuffer,
                                     m_Swapchain->GetImageAtIndex(frame.SwapchainImageIndex),
                                     VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                     VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

        DrawImGui(frame.MainCommandBuffer, m_Swapchain->GetImageViewAtIndex(frame.SwapchainImageIndex));

        // Prepare the image to be presented.
        VulkanUtils::TransitionImage(frame.MainCommandBuffer,
                                     m_Swapchain->GetImageAtIndex(frame.SwapchainImageIndex),
                                     VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

        VK_CHECK(vkEndCommandBuffer(frame.MainCommandBuffer))

        const VkCommandBufferSubmitInfo cmdInfo = VulkanInit::CommandBufferSubmitInfo(frame.MainCommandBuffer);

        const VkSemaphoreSubmitInfo waitInfo = VulkanInit::SemaphoreSubmitInfo(
            VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, frame.SwapchainSemaphore);
        const VkSemaphoreSubmitInfo signalInfo = VulkanInit::SemaphoreSubmitInfo(
            VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, frame.RenderSemaphore);

        const VkSubmitInfo2 submit = VulkanInit::SubmitInfo(&cmdInfo, &signalInfo, &waitInfo);

        VK_CHECK(vkQueueSubmit2(m_Device->GetGraphicsQueue(), 1, &submit, frame.RenderFence))

        const VkSwapchainKHR swapchain = m_Swapchain->GetSwapchain();

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.pNext = nullptr;
        presentInfo.pSwapchains = &swapchain;
        presentInfo.swapchainCount = 1;

        presentInfo.pWaitSemaphores = &frame.RenderSemaphore;
        presentInfo.waitSemaphoreCount = 1;

        presentInfo.pImageIndices = &frame.SwapchainImageIndex;

        const VkResult lastVkError = vkQueuePresentKHR(m_Device->GetPresentQueue(), &presentInfo);
        if (lastVkError == VK_ERROR_OUT_OF_DATE_KHR || lastVkError == VK_SUBOPTIMAL_KHR || window.
            ShouldInvalidateSwapchain()) {
            m_SwapchainResizeRequired = true;
        }

        if (m_SwapchainResizeRequired) {
            RecreateSwapchain(window);
            m_SwapchainResizeRequired = false;
        }

        m_FrameNumber++;
    }

    void VulkanRenderer::ImmediateSubmit(const std::function<void(VkCommandBuffer commandBuffer)>& function) const {
        VK_CHECK(vkResetFences(m_Device->GetDevice(), 1, &m_ImmediateFence))
        VK_CHECK(vkResetCommandBuffer(m_ImmediateCommandBuffer, 0))

        const VkCommandBuffer commandBuffer = m_ImmediateCommandBuffer;

        const VkCommandBufferBeginInfo cmdBeginInfo = VulkanInit::CommandBufferBeginInfo(
            VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

        VK_CHECK(vkBeginCommandBuffer(commandBuffer, &cmdBeginInfo))

        function(commandBuffer);

        VK_CHECK(vkEndCommandBuffer(commandBuffer))

        const VkCommandBufferSubmitInfo cmdInfo = VulkanInit::CommandBufferSubmitInfo(commandBuffer);
        const VkSubmitInfo2 submit = VulkanInit::SubmitInfo(&cmdInfo, nullptr, nullptr);

        // Submit command buffer to the queue and execute it.
        // m_RenderFence will now block until the graphics command finish executing.
        VK_CHECK(vkQueueSubmit2(m_Device->GetGraphicsQueue(), 1, &submit, m_ImmediateFence))

        VK_CHECK(vkWaitForFences(m_Device->GetDevice(), 1, &m_ImmediateFence, true, 9999999999))
    }

    void VulkanRenderer::InitializeVulkan(const Window& window, const DebugLevel& debugLevel) {
        m_Instance = std::make_unique<VulkanWrapper::Instance>(window, debugLevel);
        m_Device = std::make_unique<VulkanWrapper::Device>(*m_Instance);

        Log::EngineTrace("Creating VMA allocator...");

        VmaVulkanFunctions functions{};
        functions.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
        functions.vkGetDeviceProcAddr = vkGetDeviceProcAddr;

        VmaAllocatorCreateInfo allocatorInfo{};
        allocatorInfo.physicalDevice = m_Device->GetPhysicalDevice();
        allocatorInfo.device = m_Device->GetDevice();
        allocatorInfo.instance = m_Instance->GetInstance();
        allocatorInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
        allocatorInfo.pVulkanFunctions = &functions;
        vmaCreateAllocator(&allocatorInfo, &m_Allocator);

        Log::EngineTrace("VMA allocator created.");

        m_MainDeletionQueue.PushFunction([&]() {
            Log::EngineTrace("Destroying VMA allocator.");
            vmaDestroyAllocator(m_Allocator);
        });
    }

    void VulkanRenderer::InitializeSwapchain(const Window& window) {
        m_Swapchain = std::make_unique<VulkanWrapper::Swapchain>(window, *m_Instance, *m_Device);

        // Draw image size will match the window.
        const VkExtent3D drawImageExtent = {
            static_cast<u32>(window.GetWidth()), static_cast<u32>(window.GetHeight()), 1
        };

        // Hardcoding the draw format to a 32-bit float.
        DrawImage.ImageFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
        DrawImage.ImageExtent = drawImageExtent;

        VkImageUsageFlags drawImageUsages{};
        drawImageUsages |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        drawImageUsages |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        drawImageUsages |= VK_IMAGE_USAGE_STORAGE_BIT;
        drawImageUsages |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        const VkImageCreateInfo drawImageInfo = VulkanInit::ImageCreateInfo(
            DrawImage.ImageFormat, drawImageUsages, drawImageExtent);

        // For the draw image, we want to allocate it from GPU local memory.
        VmaAllocationCreateInfo drawImageAllocInfo{};
        drawImageAllocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
        drawImageAllocInfo.requiredFlags = static_cast<VkMemoryPropertyFlags>(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        // Allocate and create the image.
        vmaCreateImage(m_Allocator, &drawImageInfo, &drawImageAllocInfo, &DrawImage.Image,
                       &DrawImage.Allocation, nullptr);

        // Build an image view for the draw image to use for rendering.
        const VkImageViewCreateInfo drawImageViewInfo = VulkanInit::ImageViewCreateInfo(
            DrawImage.ImageFormat, DrawImage.Image, VK_IMAGE_ASPECT_COLOR_BIT);

        VK_CHECK(vkCreateImageView(m_Device->GetDevice(), &drawImageViewInfo, nullptr, &DrawImage.ImageView))

        DepthImage.ImageFormat = VK_FORMAT_D32_SFLOAT;
        DepthImage.ImageExtent = drawImageExtent;
        VkImageUsageFlags depthImageUsage{};
        depthImageUsage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

        VkImageCreateInfo depthImageInfo = VulkanInit::ImageCreateInfo(
            DepthImage.ImageFormat, depthImageUsage, drawImageExtent);

        // Allocate and create the image.
        vmaCreateImage(m_Allocator, &depthImageInfo, &drawImageAllocInfo, &DepthImage.Image,
                       &DepthImage.Allocation, nullptr);

        const VkImageViewCreateInfo depthImageViewInfo = VulkanInit::ImageViewCreateInfo(
            DepthImage.ImageFormat, DepthImage.Image, VK_IMAGE_ASPECT_DEPTH_BIT);

        VK_CHECK(vkCreateImageView(m_Device->GetDevice(), &depthImageViewInfo, nullptr, &DepthImage.ImageView))

        // Add to deletion queue.
        m_MainDeletionQueue.PushFunction([this]() {
            vkDestroyImageView(m_Device->GetDevice(), DrawImage.ImageView, nullptr);
            vmaDestroyImage(m_Allocator, DrawImage.Image, DrawImage.Allocation);

            vkDestroyImageView(m_Device->GetDevice(), DepthImage.ImageView, nullptr);
            vmaDestroyImage(m_Allocator, DepthImage.Image, DepthImage.Allocation);
        });
    }

    void VulkanRenderer::InitializeCommands() {
        InitializeFramesCommandBuffers();
        InitializeImmediateCommandBuffer();
    }

    void VulkanRenderer::InitializeFramesCommandBuffers() {
        const VkCommandPoolCreateInfo commandPoolInfo = VulkanInit::CommandPoolCreateInfo(
            m_Device->GetGraphicsQueueFamilyIndex(), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

        Log::EngineTrace("Creating frames Vulkan command pool & command buffer...");
        for (u32 i = 0; i < g_FrameOverlap; i++) {
            VK_CHECK(
                vkCreateCommandPool(m_Device->GetDevice(), &commandPoolInfo, nullptr, &m_Frames[i].CommandPool))

            Log::EngineTrace("Created Vulkan command pool for frame #{0}", i);

            VkCommandBufferAllocateInfo cmdAllocInfo = VulkanInit::CommandBufferAllocateInfo(
                m_Frames[i].CommandPool);

            VK_CHECK(vkAllocateCommandBuffers(m_Device->GetDevice(), &cmdAllocInfo, &m_Frames[i].MainCommandBuffer))
            Log::EngineTrace("Created Vulkan command command for frame #{0}", i);
        }

        m_MainDeletionQueue.PushFunction([this]() {
            for (u32 i = 0; i < g_FrameOverlap; i++) {
                Log::EngineTrace("Destroying Vulkan command pool for frame #{0}.", i);
                vkDestroyCommandPool(m_Device->GetDevice(), m_Frames[i].CommandPool, nullptr);
            }
        });
    }

    void VulkanRenderer::InitializeImmediateCommandBuffer() {
        // Command pool/buffer for immediate commands like copy commands.
        const VkCommandPoolCreateInfo commandPoolInfo = VulkanInit::CommandPoolCreateInfo(
            m_Device->GetGraphicsQueueFamilyIndex(),
            VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT | VK_COMMAND_POOL_CREATE_TRANSIENT_BIT);

        Log::EngineTrace("Creating Vulkan command pool for immediate command buffers...");
        VK_CHECK(vkCreateCommandPool(m_Device->GetDevice(), &commandPoolInfo, nullptr, &m_ImmediateCommandPool))
        Log::EngineTrace("Vulkan command pool for immediate command buffers created.");

        // Allocate command buffers for immediate submits.
        const VkCommandBufferAllocateInfo cmdAllocInfo = VulkanInit::CommandBufferAllocateInfo(
            m_ImmediateCommandPool);

        Log::EngineTrace("Creating Vulkan immediate command buffer...");
        VK_CHECK(vkAllocateCommandBuffers(m_Device->GetDevice(), &cmdAllocInfo, &m_ImmediateCommandBuffer))
        Log::EngineTrace("Vulkan immediate command buffer created.");

        m_MainDeletionQueue.PushFunction([this]() {
            vkDestroyCommandPool(m_Device->GetDevice(), m_ImmediateCommandPool, nullptr);
        });
    }

    void VulkanRenderer::InitializeSynchronisationPrimitives() {
        const VkFenceCreateInfo fenceCreateInfo = VulkanInit::FenceCreateInfo(VK_FENCE_CREATE_SIGNALED_BIT);
        const VkSemaphoreCreateInfo semaphoreCreateInfo = VulkanInit::SemaphoreCreateInfo();

        Log::EngineTrace("Creating Vulkan synchronisation primitives...");
        for (u32 i = 0; i < g_FrameOverlap; i++) {
            VK_CHECK(
                vkCreateFence(m_Device->GetDevice(), &fenceCreateInfo, nullptr, &m_Frames[i].RenderFence))
            Log::EngineTrace("Vulkan render fence created for frame #{0}", i);

            VK_CHECK(
                vkCreateSemaphore(m_Device->GetDevice(), &semaphoreCreateInfo, nullptr, &m_Frames[i].
                    SwapchainSemaphore))
            Log::EngineTrace("Vulkan swapchain semaphore created for frame #{0}", i);
            VK_CHECK(
                vkCreateSemaphore(m_Device->GetDevice(), &semaphoreCreateInfo, nullptr, &m_Frames[i].
                    RenderSemaphore))
            Log::EngineTrace("Vulkan render semaphore created for frame #{0}", i);
        }

        m_MainDeletionQueue.PushFunction([this]() {
            for (u32 i = 0; i < g_FrameOverlap; i++) {
                Log::EngineTrace("Destroying Vulkan synchronisation objects for frame #{0}.", i);
                vkDestroyFence(m_Device->GetDevice(), m_Frames[i].RenderFence, nullptr);
                vkDestroySemaphore(m_Device->GetDevice(), m_Frames[i].RenderSemaphore, nullptr);
                vkDestroySemaphore(m_Device->GetDevice(), m_Frames[i].SwapchainSemaphore, nullptr);

                m_Frames[i].DeletionQueue.Flush();
            }
        });

        Log::EngineTrace("Creating Vulkan fence for immediate commands...");
        VK_CHECK(vkCreateFence(m_Device->GetDevice(), &fenceCreateInfo, nullptr, &m_ImmediateFence))
        Log::EngineTrace("Vulkan fence for immediate commands created.");

        m_MainDeletionQueue.PushFunction([this]() {
            vkDestroyFence(m_Device->GetDevice(), m_ImmediateFence, nullptr);
        });
    }

    void VulkanRenderer::InitializeDescriptors() {
        // Create a descriptor pool that will hold 10 sets with 1 image each.
        std::vector<DescriptorAllocatorGrowable::PoolSizeRatio> sizes = {
            {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1}
        };

        m_GlobalDescriptorAllocator.Initialize(m_Device->GetDevice(), 10, sizes);

        // Make the descriptor set layout for the compute draw.
        {
            DescriptorLayoutBuilder builder;
            builder.AddBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
            m_DrawImageDescriptorLayout = builder.Build(m_Device->GetDevice(), VK_SHADER_STAGE_COMPUTE_BIT);
        }

        // Allocate a descriptor set for the compute draw.
        m_DrawImageDescriptors = m_GlobalDescriptorAllocator.Allocate(
            m_Device->GetDevice(), m_DrawImageDescriptorLayout);

        DescriptorWriter writer;
        writer.WriteImage(0, DrawImage.ImageView, VK_NULL_HANDLE, VK_IMAGE_LAYOUT_GENERAL,
                          VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
        writer.UpdateSet(m_Device->GetDevice(), m_DrawImageDescriptors);

        // Make sure both the descriptor allocator and the new layout get cleaned up properly.
        m_MainDeletionQueue.PushFunction([&]() {
            m_GlobalDescriptorAllocator.DestroyPools(m_Device->GetDevice());

            vkDestroyDescriptorSetLayout(m_Device->GetDevice(), m_DrawImageDescriptorLayout, nullptr);
        });

        // Create frame descriptors.
        for (auto& frame : m_Frames) {
            // Create a descriptor pool.
            std::vector<DescriptorAllocatorGrowable::PoolSizeRatio> frameSizes = {
                {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 3},
                {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 3},
                {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 3},
                {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4},
            };

            frame.FrameDescriptors = DescriptorAllocatorGrowable{};
            frame.FrameDescriptors.Initialize(m_Device->GetDevice(), 1000, frameSizes);
        }


        m_MainDeletionQueue.PushFunction([this]() {
            for (auto& frame : m_Frames) {
                frame.FrameDescriptors.DestroyPools(m_Device->GetDevice());
            }
        });

        {
            // Create scene data descriptor layout.
            DescriptorLayoutBuilder builder;
            builder.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
            GpuSceneDataLayout = builder.Build(m_Device->GetDevice(),
                                               VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);

            m_MainDeletionQueue.PushFunction([this]() {
                vkDestroyDescriptorSetLayout(m_Device->GetDevice(), GpuSceneDataLayout, nullptr);
            });
        }

        {
            DescriptorLayoutBuilder builder;
            builder.AddBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
            m_SingleImageDescriptorLayout = builder.Build(m_Device->GetDevice(), VK_SHADER_STAGE_FRAGMENT_BIT);

            m_MainDeletionQueue.PushFunction([this]() {
                vkDestroyDescriptorSetLayout(m_Device->GetDevice(), m_SingleImageDescriptorLayout, nullptr);
            });
        }
    }

    void VulkanRenderer::InitializePipelines() {
        MetalRoughMaterial.BuildPipelines(this);
    }

    void VulkanRenderer::InitializeImGui(const Window& window) {
        // 1: Create descriptor pool for ImGui
        //    The pool is very oversize, but it's copied from ImGui demo
        //    itself.
        const VkDescriptorPoolSize poolSizes[] = {
            {VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
            {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
            {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
            {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
            {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
            {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
            {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
            {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
            {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
            {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
            {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}
        };

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        poolInfo.maxSets = 1000;
        poolInfo.poolSizeCount = static_cast<u32>(std::size(poolSizes));
        poolInfo.pPoolSizes = poolSizes;

        VkDescriptorPool imguiPool;
        VK_CHECK(vkCreateDescriptorPool(m_Device->GetDevice(), &poolInfo, nullptr, &imguiPool))

        // 2: Initialize ImGui library.

        ImGui::CreateContext();

        ImGui_ImplGlfw_InitForVulkan(window.GetNativeWindow(), true);
        ImGui_ImplVulkan_InitInfo initInfo = {};
        initInfo.Instance = m_Instance->GetInstance();
        initInfo.PhysicalDevice = m_Device->GetPhysicalDevice();
        initInfo.Device = m_Device->GetDevice();
        initInfo.QueueFamily = m_Device->GetGraphicsQueueFamilyIndex();
        initInfo.Queue = m_Device->GetGraphicsQueue();
        initInfo.DescriptorPool = imguiPool;
        initInfo.MinImageCount = 3;
        initInfo.ImageCount = 3;
        initInfo.UseDynamicRendering = true;

        const VkFormat swapchainImageFormat = m_Swapchain->GetSwapchainImageFormat();
        // Dynamic rendering parameters for ImGui to use.
        initInfo.PipelineRenderingCreateInfo = {.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO};
        initInfo.PipelineRenderingCreateInfo.colorAttachmentCount = 1;
        initInfo.PipelineRenderingCreateInfo.pColorAttachmentFormats = &swapchainImageFormat;

        initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

        VkInstance instance = m_Instance->GetInstance();

        ImGui_ImplVulkan_LoadFunctions([](const char* functionName, void* vulkanInstance) {
            return vkGetInstanceProcAddr(*(static_cast<VkInstance*>(vulkanInstance)), functionName);
        }, &instance);

        ImGui_ImplVulkan_Init(&initInfo);

        ImGui_ImplVulkan_CreateFontsTexture();

        m_MainDeletionQueue.PushFunction([this, imguiPool]() {
            ImGui_ImplVulkan_Shutdown();
            vkDestroyDescriptorPool(m_Device->GetDevice(), imguiPool, nullptr);
        });
    }

    void VulkanRenderer::InitializeDefaultData() {
        // 3 default textures, white, gray, black. 1 pixel each.
        const u32 white = glm::packUnorm4x8(glm::vec4(1, 1, 1, 1));
        WhiteImage = VulkanUtils::CreateImage(m_Allocator, m_Device->GetDevice(), this,
                                              &white, VkExtent3D{1, 1, 1},
                                              VK_FORMAT_B8G8R8A8_UNORM, VK_IMAGE_USAGE_SAMPLED_BIT);

        const u32 gray = glm::packUnorm4x8(glm::vec4(0.66f, 0.66f, 0.66f, 1));
        GrayImage = VulkanUtils::CreateImage(m_Allocator, m_Device->GetDevice(), this,
                                             &gray, VkExtent3D{1, 1, 1},
                                             VK_FORMAT_B8G8R8A8_UNORM, VK_IMAGE_USAGE_SAMPLED_BIT);

        const u32 black = glm::packUnorm4x8(glm::vec4(0, 0, 0, 1));
        BlackImage = VulkanUtils::CreateImage(m_Allocator, m_Device->GetDevice(), this,
                                              &black, VkExtent3D{1, 1, 1},
                                              VK_FORMAT_B8G8R8A8_UNORM, VK_IMAGE_USAGE_SAMPLED_BIT);

        // Checkerboard image.
        const u32 magenta = glm::packUnorm4x8(glm::vec4(1, 0, 1, 1));
        std::array<u32, static_cast<size_t>(16) * 16> pixels; // For 16x16 checkerboard texture.
        for (i32 x = 0; x < 16; x++) {
            for (i32 y = 0; y < 16; y++) {
                pixels[y * 16 + x] = ((x % 2) ^ (y % 2)) ? magenta : black;
            }
        }

        ErrorCheckerboardImage = VulkanUtils::CreateImage(m_Allocator, m_Device->GetDevice(), this, pixels.data(),
                                                          VkExtent3D{16, 16, 1}, VK_FORMAT_B8G8R8A8_UNORM,
                                                          VK_IMAGE_USAGE_SAMPLED_BIT);

        VkSamplerCreateInfo samplerInfo = {.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO};
        samplerInfo.magFilter = VK_FILTER_NEAREST;
        samplerInfo.minFilter = VK_FILTER_NEAREST;

        vkCreateSampler(m_Device->GetDevice(), &samplerInfo, nullptr, &DefaultSamplerNearest);

        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;

        vkCreateSampler(m_Device->GetDevice(), &samplerInfo, nullptr, &DefaultSamplerLinear);

        m_MainDeletionQueue.PushFunction([this]() {
            vkDestroySampler(m_Device->GetDevice(), DefaultSamplerNearest, nullptr);
            vkDestroySampler(m_Device->GetDevice(), DefaultSamplerLinear, nullptr);

            VulkanUtils::DestroyImage(m_Allocator, m_Device->GetDevice(), WhiteImage);
            VulkanUtils::DestroyImage(m_Allocator, m_Device->GetDevice(), GrayImage);
            VulkanUtils::DestroyImage(m_Allocator, m_Device->GetDevice(), BlackImage);
            VulkanUtils::DestroyImage(m_Allocator, m_Device->GetDevice(), ErrorCheckerboardImage);
        });

        GLTFMetallic_Roughness::MaterialResources materialResources;
        materialResources.ColorImage = WhiteImage;
        materialResources.ColorSampler = DefaultSamplerLinear;
        materialResources.MetalRoughnessImage = WhiteImage;
        materialResources.MetalRoughnessSampler = DefaultSamplerLinear;

        const VulkanUtils::AllocatedBuffer materialConstants = VulkanUtils::CreateBuffer(
            m_Allocator, sizeof(GLTFMetallic_Roughness::MaterialConstants), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VulkanUtils::MemoryUsage::CpuToGpu);

        VmaAllocationInfo materialConstantsAllocInfo;
        vmaGetAllocationInfo(m_Allocator, materialConstants.Allocation, &materialConstantsAllocInfo);
        
        auto sceneUniformData = static_cast<GLTFMetallic_Roughness::MaterialConstants*>(materialConstantsAllocInfo.pMappedData);
        sceneUniformData->ColorFactors = glm::vec4(1, 1, 1, 1);
        sceneUniformData->MetalRoughFactors = glm::vec4(1, 0.5, 0, 0);

        m_MainDeletionQueue.PushFunction([this, materialConstants]() {
            VulkanUtils::DestroyBuffer(m_Allocator, materialConstants);
        });

        materialResources.DataBuffer = materialConstants.Buffer;
        materialResources.DataBufferOffset = 0;

        m_DefaultData = MetalRoughMaterial.WriteMaterial(m_Device->GetDevice(), MaterialPass::MainColor,
                                                         materialResources, m_GlobalDescriptorAllocator);
    }

    void VulkanRenderer::DrawImGui(const VkCommandBuffer commandBuffer, const VkImageView targetImageView) const {
        const VkRenderingAttachmentInfo colorAttachment = VulkanInit::AttachmentInfo(targetImageView, nullptr,
                                                                                     VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
        const VkRenderingInfo renderInfo = VulkanInit::RenderingInfo(m_Swapchain->GetSwapchainExtent(),
                                                                     &colorAttachment, nullptr);

        vkCmdBeginRendering(commandBuffer, &renderInfo);

        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);

        vkCmdEndRendering(commandBuffer);
    }

    void VulkanRenderer::RecreateSwapchain(Window& window) {
        vkDeviceWaitIdle(m_Device->GetDevice());

        m_Swapchain->Destroy();

        m_Swapchain = std::make_unique<VulkanWrapper::Swapchain>(window, *m_Instance, *m_Device);

        window.SwapchainInvalidated();
    }

    bool VulkanRenderer::IsVisible(const RenderObject& object, const glm::mat4& viewProj) {
        constexpr std::array<glm::vec3, 8> corners{
            glm::vec3{1, 1, 1},
            glm::vec3{1, 1, -1},
            glm::vec3{1, -1, 1},
            glm::vec3{1, -1, -1},
            glm::vec3{-1, 1, 1},
            glm::vec3{-1, 1, -1},
            glm::vec3{-1, -1, 1},
            glm::vec3{-1, -1, -1},
        };

        const glm::mat4 matrix = viewProj * object.Transform;

        glm::vec3 min = {1.5, 1.5, 1.5};
        glm::vec3 max = {-1.5, -1.5, -1.5};

        for (int c = 0; c < 8; c++) {
            // project each corner into clip space
            glm::vec4 v = matrix * glm::vec4(object.Bounds.Origin + (corners[c] * object.Bounds.Extents), 1.f);

            // perspective correction
            v.x = v.x / v.w;
            v.y = v.y / v.w;
            v.z = v.z / v.w;

            min = glm::min(glm::vec3{v.x, v.y, v.z}, min);
            max = glm::max(glm::vec3{v.x, v.y, v.z}, max);
        }

        // check the clip space box is within the view
        if (min.z > 1.f || max.z < 0.f || min.x > 1.f || max.x < -1.f || min.y > 1.f || max.y < -1.f) {
            return false;
        }
        return true;
    }
}
