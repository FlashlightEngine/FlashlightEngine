#include "TestApplication.hpp"


bool TestApplication::Init(const Flashlight::WindowProperties& windowProperties,
                           const Flashlight::DebugLevel& debugLevel) {
    if (!Application::Init(windowProperties, debugLevel)) {
        Flashlight::Log::AppFatal({0x00, 0x01}, "Failed to initialize application base.");
    }

    std::vector<Flashlight::Vertex2DColor> vertices = {
        {{ -0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f }},
        {{  0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f }},
        {{  0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f }},
        {{ -0.5f,  0.5f }, { 1.0f, 1.0f, 1.0f }}
    };

    std::vector<u32> indices = {
        0, 1, 2, 2, 3, 0
    };

    m_RectangleMesh = std::make_unique<Flashlight::Mesh<Flashlight::Vertex2DColor>>(m_Renderer->GetDevice(), vertices, indices);

    m_IsRunning = true;

    return true;
}

void TestApplication::Update() {
    Application::Update();
}

void TestApplication::Render() {
    VkCommandBuffer commandBuffer = m_Renderer->BeginFrame();
    if (commandBuffer) {
        m_Renderer->BeginRenderPass(commandBuffer);

        m_Renderer->UseMainPipeline(commandBuffer);

        m_RectangleMesh->Draw(commandBuffer);

        m_Renderer->EndRenderPass(commandBuffer);

        m_Renderer->EndFrame();
    }
}

void TestApplication::Cleanup() {
}
