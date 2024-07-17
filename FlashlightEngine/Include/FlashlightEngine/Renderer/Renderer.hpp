#pragma once

#include "FlashlightEngine/Renderer/WGPUWrapper/Instance.hpp"

#include "FlashlightEngine/pch.hpp"

namespace Flashlight {
    class Renderer {
        std::unique_ptr<WGPUWrapper::Instance> m_Instance;

    public:
        Renderer();
        ~Renderer() = default;

        void Initialize();
    };

#include "Renderer.inl"
}
