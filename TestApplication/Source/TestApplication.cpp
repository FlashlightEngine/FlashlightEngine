#include "TestApplication.hpp"


bool TestApplication::Init(const Flashlight::WindowProperties& windowProperties) {
    if (!Application::Init(windowProperties)) {
        Flashlight::Log::AppFatal({0x00, 0x01}, "Failed to initialize application base.");
    }

    Flashlight::VertexPositionColor vertices[] = {
        {Flashlight::VertexPosition{ 0.0f,  0.5f, 0.0f}, Flashlight::VertexColor{0.25f, 0.39f, 0.19f}},
        {Flashlight::VertexPosition{-0.5f, -0.5f, 0.0f}, Flashlight::VertexColor{0.44f, 0.75f, 0.35f}},
        {Flashlight::VertexPosition{ 0.5f, -0.5f, 0.0f}, Flashlight::VertexColor{0.38f, 0.55f, 0.20f}}
    };

    D3D11_BUFFER_DESC bufferInfo = {};
    bufferInfo.ByteWidth = sizeof(vertices);
    bufferInfo.Usage = D3D11_USAGE_IMMUTABLE;
    bufferInfo.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA resourceData = {};
    resourceData.pSysMem = vertices;

    if (FAILED(m_Renderer->GetDevice()->CreateBuffer(&bufferInfo, &resourceData, &m_TriangleVertices))) {
        Flashlight::Log::EngineError("Failed to create vertex buffer.");
    }
    
    m_IsRunning = true;

    return true;
}

void TestApplication::Update() {
    Application::Update();
}

void TestApplication::Render() {
    m_Renderer->BeginRendering(*m_Window);

    u32 stride = m_Renderer->GetMainShader()->GetLayoutByteSize(Flashlight::VertexType::PositionColor);
    constexpr u32 vertexOffset = 0;
    m_Renderer->GetDeviceContext()->IASetVertexBuffers(0, 1, m_TriangleVertices.GetAddressOf(), &stride, &vertexOffset);
    m_Renderer->GetDeviceContext()->Draw(3, 0);
    
    m_Renderer->EndRendering();
}

void TestApplication::Cleanup() {
    m_TriangleVertices.Reset();
}