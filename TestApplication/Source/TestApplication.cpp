#include "TestApplication.hpp"


bool TestApplication::Init() {
    const std::vector<Flashlight::Vertex> triangleVertices = {
        {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
        {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
    };

    m_TriangleModel = std::make_unique<Flashlight::Model>(triangleVertices);

    m_IsRunning = true;

    return true;
}

void TestApplication::Update() {
}

void TestApplication::Render() {
    const VkCommandBuffer commandBuffer = m_Renderer->BeginFrame();

    if (commandBuffer) {
        m_Renderer->BeginRenderPass(commandBuffer);

        m_Renderer->GetPipeline().Bind(commandBuffer);

        vkCmdDraw(commandBuffer, 3, 1, 0, 0);

        m_Renderer->EndRenderPass(commandBuffer);

        m_Renderer->EndFrame();
    }
}

void TestApplication::Cleanup() {
}
