#pragma once

#include "FlashlightEngine/Renderer/WGPUWrapper/Instance.hpp"
#include "FlashlightEngine/Renderer/WGPUWrapper/Device.hpp"

#include "FlashlightEngine/pch.hpp"

namespace Flashlight {
    class Renderer {
        std::unique_ptr<WGPUWrapper::Instance> m_Instance;
        std::unique_ptr<WGPUWrapper::Device> m_Device;

    public:
        Renderer();
        ~Renderer() = default;

        static void Initialize();
    };

#include "Renderer.inl"
}
