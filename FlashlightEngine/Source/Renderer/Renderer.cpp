#include "FlashlightEngine/Renderer/Renderer.hpp"

namespace Flashlight {
    Renderer::Renderer() {
        m_Instance = std::make_unique<WGPUWrapper::Instance>();
        m_Device = std::make_unique<WGPUWrapper::Device>(*m_Instance);
    }


    void Renderer::Initialize() {
    }
}
