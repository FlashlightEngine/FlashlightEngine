/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : VulkanRenderer.cpp
 * Description : Definitions of methods from the VulkanRenderer class.
 */
#include <FlashlightEngine/VulkanRenderer/VulkanRenderer.hpp>

#include <FlashlightEngine/VulkanRenderer/VulkanInitializers.hpp>
#include <FlashlightEngine/VulkanRenderer/VulkanUtils/VulkanImageUtils.hpp>

#include <magic_enum.hpp>


#include <SDL.h>

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

#include <imgui.h>
#include <imgui_impl_vulkan.h>
#include <imgui_impl_sdl2.h>

namespace Flashlight::VulkanRenderer {

    VulkanRenderer::VulkanRenderer(const Window& window, const DebugLevel& debugLevel) {
        InitializeVulkan(window, debugLevel);
        InitializeSwapchain(window);
        InitializeCommands();
        InitializeSynchronisationPrimitives();
        InitializeDescriptors();
        InitializePipelines();
        InitializeImGui(window);

        m_RendererInitialized = true;
    }

    VulkanRenderer::~VulkanRenderer() {
        vkDeviceWaitIdle(m_Device->GetDevice());

        if (m_RendererInitialized) {
            m_MainDeletionQueue.Flush();

            m_RendererInitialized = false;
        }
    }

    void VulkanRenderer::CreateUi() {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplSDL2_NewFrame();

        ImGui::NewFrame();

        if (ImGui::Begin("Background effect data")) {
            ComputeEffect& selected = m_BackgroundEffects[m_CurrentBackgroundEffect];

            ImGui::Text("Selected effect: %s", selected.Name);

            ImGui::SliderInt("Effect Index", &m_CurrentBackgroundEffect, 0,
                             static_cast<i32>(m_BackgroundEffects.size() - 1));

            ImGui::InputFloat4("data1", reinterpret_cast<float*>(&selected.Data.Data1));
            ImGui::InputFloat4("data2", reinterpret_cast<float*>(&selected.Data.Data2));
            ImGui::InputFloat4("data3", reinterpret_cast<float*>(&selected.Data.Data3));
            ImGui::InputFloat4("data4", reinterpret_cast<float*>(&selected.Data.Data4));
        }
        ImGui::End();

        ImGui::Render();
    }


    void VulkanRenderer::Draw() {
        auto& frame = GetCurrentFrame();

        m_Swapchain->AcquireNextImage(*m_Device, frame);

        const VkCommandBuffer cmd = frame.MainCommandBuffer;

        VK_CHECK(vkResetCommandBuffer(cmd, 0))

        const VkCommandBufferBeginInfo cmdBeginInfo = VulkanInit::CommandBufferBeginInfo(
            VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

        m_DrawExtent.width = m_DrawImage.ImageExtent.width;
        m_DrawExtent.height = m_DrawImage.ImageExtent.height;

        VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo))

        VulkanUtils::TransitionImage(cmd, m_DrawImage.Image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

        DrawBackground(cmd);

        // Transition the draw image and the swap chain image into their correct transfer layouts.
        VulkanUtils::TransitionImage(frame.MainCommandBuffer, m_DrawImage.Image, VK_IMAGE_LAYOUT_GENERAL,
                                     VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

        DrawGeometry(cmd);

        VulkanUtils::TransitionImage(frame.MainCommandBuffer, m_DrawImage.Image,
                                     VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                     VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

        VulkanUtils::TransitionImage(frame.MainCommandBuffer,
                                     m_Swapchain->GetImageAtIndex(frame.SwapchainImageIndex),
                                     VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

        VulkanUtils::CopyImageToImage(frame.MainCommandBuffer, m_DrawImage.Image,
                                      m_Swapchain->GetImageAtIndex(frame.SwapchainImageIndex), m_DrawExtent,
                                      m_Swapchain->GetSwapchainExtent());

        VulkanUtils::TransitionImage(frame.MainCommandBuffer,
                                     m_Swapchain->GetImageAtIndex(frame.SwapchainImageIndex),
                                     VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                     VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

        DrawImGui(cmd, m_Swapchain->GetImageViewAtIndex(frame.SwapchainImageIndex));

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

        VK_CHECK(vkQueuePresentKHR(m_Device->GetPresentQueue(), &presentInfo))

        m_FrameNumber++;
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
        m_DrawImage.ImageFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
        m_DrawImage.ImageExtent = drawImageExtent;

        VkImageUsageFlags drawImageUsages{};
        drawImageUsages |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        drawImageUsages |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        drawImageUsages |= VK_IMAGE_USAGE_STORAGE_BIT;
        drawImageUsages |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        const VkImageCreateInfo drawImageInfo = VulkanInit::ImageCreateInfo(
            m_DrawImage.ImageFormat, drawImageUsages, drawImageExtent);

        // For the draw image, we want to allocate it from GPU local memory.
        VmaAllocationCreateInfo drawImageAllocInfo{};
        drawImageAllocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
        drawImageAllocInfo.requiredFlags = static_cast<VkMemoryPropertyFlags>(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        // Allocate and create the image.
        vmaCreateImage(m_Allocator, &drawImageInfo, &drawImageAllocInfo, &m_DrawImage.Image,
                       &m_DrawImage.Allocation, nullptr);

        // Build an image view for the draw image to use for rendering.
        const VkImageViewCreateInfo drawImageViewInfo = VulkanInit::ImageViewCreateInfo(
            m_DrawImage.ImageFormat, m_DrawImage.Image, VK_IMAGE_ASPECT_COLOR_BIT);

        VK_CHECK(vkCreateImageView(m_Device->GetDevice(), &drawImageViewInfo, nullptr, &m_DrawImage.ImageView))

        // Add to deletion queue.
        m_MainDeletionQueue.PushFunction([this]() {
            vkDestroyImageView(m_Device->GetDevice(), m_DrawImage.ImageView, nullptr);
            vmaDestroyImage(m_Allocator, m_DrawImage.Image, m_DrawImage.Allocation);
        });
    }

    void VulkanRenderer::InitializeCommands() {
        VkCommandPoolCreateInfo commandPoolInfo = VulkanInit::CommandPoolCreateInfo(
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

        // Command pool/buffer for immediate commands like copy commands.
        commandPoolInfo = VulkanInit::CommandPoolCreateInfo(m_Device->GetGraphicsQueueFamilyIndex(),
                                                            VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT |
                                                            VK_COMMAND_POOL_CREATE_TRANSIENT_BIT);

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
        std::vector<DescriptorAllocator::PoolSizeRatio> sizes = {
            {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1}
        };

        m_GlobalDescriptorAllocator.InitPool(m_Device->GetDevice(), 10, sizes);

        // Make the descriptor set layout for the compute draw.
        {
            DescriptorLayoutBuilder builder;
            builder.AddBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
            m_DrawImageDescriptorLayout = builder.Build(m_Device->GetDevice(), VK_SHADER_STAGE_COMPUTE_BIT);
        }

        // Allocate a descriptor set for the compute draw.
        m_DrawImageDescriptors = m_GlobalDescriptorAllocator.Allocate(
            m_Device->GetDevice(), m_DrawImageDescriptorLayout);

        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
        imageInfo.imageView = m_DrawImage.ImageView;

        VkWriteDescriptorSet drawImageWrite{};
        drawImageWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        drawImageWrite.pNext = nullptr;

        drawImageWrite.dstBinding = 0;
        drawImageWrite.dstSet = m_DrawImageDescriptors;
        drawImageWrite.descriptorCount = 1;
        drawImageWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        drawImageWrite.pImageInfo = &imageInfo;

        vkUpdateDescriptorSets(m_Device->GetDevice(), 1, &drawImageWrite, 0, nullptr);

        // Make sure both the descriptor allocator and the new layout get cleaned up properly.
        m_MainDeletionQueue.PushFunction([&]() {
            m_GlobalDescriptorAllocator.DestroyPool(m_Device->GetDevice());

            vkDestroyDescriptorSetLayout(m_Device->GetDevice(), m_DrawImageDescriptorLayout, nullptr);
        });
    }

    void VulkanRenderer::InitializePipelines() {
        InitializeComputePipelines();
        InitializeTrianglePipeline();
    }

    void VulkanRenderer::InitializeComputePipelines() {
        // Create pipeline layout
        VkPipelineLayoutCreateInfo computeLayout{};
        computeLayout.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        computeLayout.pNext = nullptr;

        computeLayout.setLayoutCount = 1;
        computeLayout.pSetLayouts = &m_DrawImageDescriptorLayout;

        VkPushConstantRange pushConstant;
        pushConstant.offset = 0;
        pushConstant.size = sizeof(ComputePushConstants);
        pushConstant.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

        computeLayout.pushConstantRangeCount = 1;
        computeLayout.pPushConstantRanges = &pushConstant;

        VK_CHECK(vkCreatePipelineLayout(m_Device->GetDevice(), &computeLayout, nullptr, &m_ComputePipelineLayout))

        // Create shader modules.
        VkShaderModule gradientShaderModule;
        CreateShaderModule(m_Device->GetDevice(), "Shaders/gradient_color.comp", VulkanUtils::ShaderType::Compute,
                           &gradientShaderModule);
        if (!gradientShaderModule) {
            Log::EngineFatal({0x02, 0x00}, "Failed to build the gradient compute shader module.");
        }

        VkShaderModule skyShaderModule;
        CreateShaderModule(m_Device->GetDevice(), "Shaders/sky.comp", VulkanUtils::ShaderType::Compute,
                           &skyShaderModule);
        if (!skyShaderModule) {
            Log::EngineFatal({0x02, 0x01}, "Failed to build the sky compute shader module.");
        }

        VkShaderModule gridShaderModule;
        CreateShaderModule(m_Device->GetDevice(), "Shaders/gradient_grid.comp", VulkanUtils::ShaderType::Compute,
                           &gridShaderModule);
        if (!skyShaderModule) {
            Log::EngineFatal({0x02, 0x02}, "Failed to build the grid compute shader module.");
        }

        VkPipelineShaderStageCreateInfo stageInfo{};
        stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        stageInfo.pNext = nullptr;
        stageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
        stageInfo.module = gradientShaderModule;
        stageInfo.pName = "main";


        VkComputePipelineCreateInfo computePipelineCreateInfo{};
        computePipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
        computePipelineCreateInfo.pNext = nullptr;
        computePipelineCreateInfo.layout = m_ComputePipelineLayout;
        computePipelineCreateInfo.stage = stageInfo;

        ComputeEffect gradient;
        gradient.Layout = m_ComputePipelineLayout;
        gradient.Name = "Gradient";
        gradient.Data = {};

        // default colors
        gradient.Data.Data1 = glm::vec4(1, 0, 0, 1);
        gradient.Data.Data2 = glm::vec4(0, 0, 1, 1);

        VK_CHECK(
            vkCreateComputePipelines(m_Device->GetDevice(), VK_NULL_HANDLE, 1, &computePipelineCreateInfo, nullptr,
                &gradient.Pipeline))

        computePipelineCreateInfo.stage.module = skyShaderModule;

        ComputeEffect sky;
        sky.Layout = m_ComputePipelineLayout;
        sky.Name = "Sky";
        sky.Data = {};

        // Default sky parameters.
        sky.Data.Data1 = glm::vec4(0.1, 0.2, 0.4, 0.97);

        VK_CHECK(
            vkCreateComputePipelines(m_Device->GetDevice(), VK_NULL_HANDLE, 1, &computePipelineCreateInfo, nullptr,
                &sky.Pipeline))

        computePipelineCreateInfo.stage.module = gridShaderModule;

        ComputeEffect grid;
        grid.Layout = m_ComputePipelineLayout;
        grid.Name = "Grid";
        grid.Data = {};

        VK_CHECK(
            vkCreateComputePipelines(m_Device->GetDevice(), VK_NULL_HANDLE, 1, &computePipelineCreateInfo, nullptr,
                &grid.Pipeline))

        // Add the 2 effects into the array.
        m_BackgroundEffects.push_back(gradient);
        m_BackgroundEffects.push_back(sky);
        m_BackgroundEffects.push_back(grid);

        vkDestroyShaderModule(m_Device->GetDevice(), gradientShaderModule, nullptr);
        vkDestroyShaderModule(m_Device->GetDevice(), skyShaderModule, nullptr);
        vkDestroyShaderModule(m_Device->GetDevice(), gridShaderModule, nullptr);

        m_MainDeletionQueue.PushFunction([this, sky, gradient, grid]() {
            vkDestroyPipelineLayout(m_Device->GetDevice(), m_ComputePipelineLayout, nullptr);
            vkDestroyPipeline(m_Device->GetDevice(), sky.Pipeline, nullptr);
            vkDestroyPipeline(m_Device->GetDevice(), gradient.Pipeline, nullptr);
            vkDestroyPipeline(m_Device->GetDevice(), grid.Pipeline, nullptr);
        });
    }

    void VulkanRenderer::InitializeTrianglePipeline() {
        VkShaderModule triangleVertexShader;
        VulkanUtils::CreateShaderModule(m_Device->GetDevice(), "Shaders/colored_triangle.vert",
                                        VulkanUtils::ShaderType::Vertex, &triangleVertexShader);
        if (!triangleVertexShader) {
            Log::EngineFatal({0x02, 0x03}, "Failed to create triangle vertex shader.");
        }

        VkShaderModule triangleFragmentShader;
        VulkanUtils::CreateShaderModule(m_Device->GetDevice(), "Shaders/colored_triangle.frag",
                                        VulkanUtils::ShaderType::Fragment, &triangleFragmentShader);
        if (!triangleFragmentShader) {
            Log::EngineFatal({0x02, 0x03}, "Failed to create triangle fragment shader.");
        }

        const VkPipelineLayoutCreateInfo pipelineLayoutInfo = VulkanInit::PipelineLayoutCreateInfo();
        VK_CHECK(
            vkCreatePipelineLayout(m_Device->GetDevice(), &pipelineLayoutInfo, nullptr, &m_TrianglePipelineLayout))

        VulkanUtils::PipelineBuilder pipelineBuilder{};

        pipelineBuilder.SetPipelineLayout(m_TrianglePipelineLayout);
        pipelineBuilder.SetShaders(triangleVertexShader, triangleFragmentShader);
        pipelineBuilder.SetInputTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
        pipelineBuilder.SetPolygonMode(VK_POLYGON_MODE_FILL);
        pipelineBuilder.SetCullMode(VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE);
        pipelineBuilder.SetMultisamplingNone();
        pipelineBuilder.DisableBlending();
        pipelineBuilder.DisableDepthTest();

        pipelineBuilder.SetColorAttachmentFormat(m_DrawImage.ImageFormat);
        pipelineBuilder.SetDepthFormat(VK_FORMAT_UNDEFINED);

        m_TrianglePipeline = pipelineBuilder.BuildPipeline(m_Device->GetDevice());

        vkDestroyShaderModule(m_Device->GetDevice(), triangleVertexShader, nullptr);
        vkDestroyShaderModule(m_Device->GetDevice(), triangleFragmentShader, nullptr);

        m_MainDeletionQueue.PushFunction([&]() {
            vkDestroyPipelineLayout(m_Device->GetDevice(), m_TrianglePipelineLayout, nullptr);
            vkDestroyPipeline(m_Device->GetDevice(), m_TrianglePipeline, nullptr);
        });
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

        ImGui_ImplSDL2_InitForVulkan(window.GetNativeWindow());

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

    void VulkanRenderer::DrawBackground(const VkCommandBuffer commandBuffer) const {
        const ComputeEffect& effect = m_BackgroundEffects[m_CurrentBackgroundEffect];

        // Bind the gradient drawing compute pipeline.
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, effect.Pipeline);

        // Bind the descriptor set containing the draw image for the compute pipeline.
        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, m_ComputePipelineLayout, 0, 1,
                                &m_DrawImageDescriptors, 0, nullptr);

        vkCmdPushConstants(commandBuffer, m_ComputePipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0,
                           sizeof(ComputePushConstants), &effect.Data);

        // Execute the compute pipeline dispatch. The workgroup size is 16x16, so we need to divide by it.
        vkCmdDispatch(commandBuffer, static_cast<u32>(std::ceil(m_DrawExtent.width / 16.0)),
                      static_cast<u32>(std::ceil(m_DrawExtent.height / 16.0)), 1);
    }

    void VulkanRenderer::DrawGeometry(const VkCommandBuffer commandBuffer) const {
        // Begin a render pass connected to the draw image.
        const VkRenderingAttachmentInfo colorAttachment = VulkanInit::AttachmentInfo(
            m_DrawImage.ImageView, nullptr, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

        const VkRenderingInfo renderingInfo = VulkanInit::RenderingInfo(m_DrawExtent, &colorAttachment, nullptr);
        vkCmdBeginRendering(commandBuffer, &renderingInfo);

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_TrianglePipeline);

        VkViewport viewport;
        viewport.x = 0;
        viewport.y = 0;
        viewport.width = static_cast<f32>(m_DrawExtent.width);
        viewport.height = static_cast<f32>(m_DrawExtent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

        VkRect2D scissor;
        scissor.offset.x = 0;
        scissor.offset.y = 0;
        scissor.extent.width = m_DrawExtent.width;
        scissor.extent.height = m_DrawExtent.height;

        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

        vkCmdDraw(commandBuffer, 3, 1, 0, 0);

        vkCmdEndRendering(commandBuffer);
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
}
