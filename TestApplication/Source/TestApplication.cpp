#include "TestApplication.hpp"

#include "FlashlightEngine/Renderer/WGPUWrapper/CommandEncoder.hpp"

bool TestApplication::Init() {
    const Flashlight::WindowProperties windowProperties {640, 480, "Test Application"};

    m_Window = std::make_unique<Flashlight::Window>(windowProperties);
    m_Renderer = std::make_shared<Flashlight::Renderer>();

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
    const Flashlight::WGPUWrapper::CommandEncoder commandEncoder = m_Renderer->BeginRecordCommands();

    wgpuCommandEncoderInsertDebugMarker(commandEncoder.GetNativeEncoder(), "One thing");
    wgpuCommandEncoderInsertDebugMarker(commandEncoder.GetNativeEncoder(), "Another thing");

    WGPUCommandBuffer commandBuffer = m_Renderer->EndRecordCommands(commandEncoder);

    m_Renderer->SubmitCommandBuffers({commandBuffer});
}

void TestApplication::Cleanup() {

}
