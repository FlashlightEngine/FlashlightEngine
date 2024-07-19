#include "TestApplication.hpp"

#include "FlashlightEngine/Core/Logger.hpp"

#include "FlashlightEngine/Renderer/WGPUWrapper/CommandEncoder.hpp"

bool TestApplication::Init() {
    m_Renderer = std::make_shared<Flashlight::Renderer>();

    m_IsRunning = true;

    return true;
}

void TestApplication::Update() {
    Close();
}

void TestApplication::Render() {
    const Flashlight::WGPUWrapper::CommandEncoder commandEncoder = m_Renderer->BeginRecordCommands();

    wgpuCommandEncoderInsertDebugMarker(commandEncoder.GetNativeEncoder(), "One thing");
    wgpuCommandEncoderInsertDebugMarker(commandEncoder.GetNativeEncoder(), "Another thing");

    WGPUCommandBuffer commandBuffer = m_Renderer->EndRecordCommands(commandEncoder);

    Flashlight::Log::AppTrace("Submitting command buffer...");
    m_Renderer->SubmitCommandBuffers({commandBuffer});
    Flashlight::Log::AppTrace("Command buffer submitted.");
}

void TestApplication::Cleanup() {

}
