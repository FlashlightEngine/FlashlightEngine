#include "TestApplication.hpp"


bool TestApplication::Init() {
    Flashlight::RenderPassInfo renderPassInfo{};
    renderPassInfo = Flashlight::RenderPass::UseDefaultRenderPassInfo(m_Renderer->GetSwapChainImageFormat());
    m_RenderPass = m_Renderer->CreateRenderPass(renderPassInfo);

    Flashlight::PipelineInfos pipelineInfos{};
    Flashlight::GraphicsPipeline::UseDefaultPipelineInfos(pipelineInfos, *m_RenderPass);

    // Shader paths still need to be configured manually.
    pipelineInfos.VertexShaderPath = "Shaders/basic.vert";
    pipelineInfos.FragmentShaderPath = "Shaders/basic.frag";
    
    m_GraphicsPipeline = m_Renderer->CreatePipeline(pipelineInfos);

    m_IsRunning = true;

    return true;
}

void TestApplication::Update() {
}

void TestApplication::Render() {
}

void TestApplication::Cleanup() {
}
