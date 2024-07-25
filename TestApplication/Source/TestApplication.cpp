#include "TestApplication.hpp"


bool TestApplication::Init() {
    const std::vector<Flashlight::Vertex> triangleVertices = {
        {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
        {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
    };

    m_TriangleMesh = std::make_unique<Flashlight::Mesh>(m_Renderer->GetDevice(), triangleVertices);

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

        m_TriangleMesh->Bind(commandBuffer);
        m_TriangleMesh->Draw(commandBuffer);

        m_Renderer->EndRenderPass(commandBuffer);

        m_Renderer->EndFrame();
    }
}

void TestApplication::Cleanup() {
}
