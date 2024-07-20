#include "TestApplication.hpp"

#include "FlashlightEngine/Renderer/CommandEncoder.hpp"
#include "FlashlightEngine/Renderer/RenderPass.hpp"

bool TestApplication::Init() {
    m_IsRunning = true;

    return true;
}

void TestApplication::Update() {
    m_Window->Update();

    if (m_Window->ShouldClose()) {
        m_IsRunning = false;
    }
}

void TestApplication::Render() {
    const auto targetView = m_Renderer->GetNextSwapChainTextureView();
    if (!targetView) {
        Flashlight::Log::AppError("Couldn't get valid texture view.");
    }

    const Flashlight::CommandEncoder commandEncoder = m_Renderer->BeginRecordCommands();
    {
        Flashlight::RenderPass renderPass{};

        renderPass.Begin(commandEncoder, targetView, {0.1, 0.1, 0.1, 1.0});

        renderPass.End();
    }
    WGPUCommandBuffer commandBuffer = m_Renderer->EndRecordCommands(commandEncoder);

    m_Renderer->SubmitCommandBuffers({commandBuffer});

    m_Renderer->ReleaseTextureView(targetView);

    m_Renderer->PresentSurface();
}

void TestApplication::Cleanup() {
}
