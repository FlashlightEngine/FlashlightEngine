#include "TestApplication.hpp"


bool TestApplication::Init() {
    Flashlight::PipelineInfos pipelineInfos{};
    Flashlight::GraphicsPipeline::UseDefaultPipelineInfos(pipelineInfos);
    m_GraphicsPipeline = m_Renderer->CreatePipeline("Shaders/basic.vert", "Shaders/basic.frag", pipelineInfos);

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
}

void TestApplication::Cleanup() {
}
