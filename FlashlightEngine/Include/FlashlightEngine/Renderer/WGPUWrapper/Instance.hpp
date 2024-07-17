#pragma once

#include "FlashlightEngine/pch.hpp"

namespace Flashlight::WGPUWrapper {

    class Instance {
        WGPUInstance m_Instance = nullptr;

    public:
        inline Instance();
        inline ~Instance();

        Instance(const Instance &) = delete;
        Instance(Instance &&) = delete;

        Instance &operator=(const Instance &) = delete;
        Instance &operator=(Instance&&) = delete;

        [[nodiscard]] inline WGPUInstance GetNativeInstance() const;

    private:
        void Create();
        void Destroy() const;
    };

#include "Instance.inl"

}