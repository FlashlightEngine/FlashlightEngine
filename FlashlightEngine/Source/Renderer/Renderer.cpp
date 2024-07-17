#include "FlashlightEngine/Renderer/Renderer.hpp"

namespace Flashlight {
    Renderer::Renderer() {
        m_Instance = std::make_unique<WGPUWrapper::Instance>();
    }


    void Renderer::Initialize() {
    }
}
