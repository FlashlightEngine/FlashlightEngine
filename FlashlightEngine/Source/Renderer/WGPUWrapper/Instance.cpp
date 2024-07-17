#include "FlashlightEngine/Renderer/WGPUWrapper/Instance.hpp"

#include "FlashlightEngine/Core/Logger.hpp"

namespace Flashlight::WGPUWrapper {

    void Instance::Create() {
        WGPUInstanceDescriptor instanceDescriptor{};
        instanceDescriptor.nextInChain = nullptr;

        m_Instance = wgpuCreateInstance(&instanceDescriptor);

        if (!m_Instance) {
            Log::EngineFatal({0x01, 0x01}, "Failed to create WebGPU instance.");
        }
        Log::EngineTrace("WebGPU instance created.");
    }

    void Instance::Destroy() const {
        if (m_Instance) {
            Log::EngineTrace("Destroying WebGPU instance.");
            wgpuInstanceRelease(m_Instance);
        }
    }

}