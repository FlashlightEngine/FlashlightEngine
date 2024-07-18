#pragma once

#include "Instance.hpp"

#include "FlashlightEngine/pch.hpp"

namespace Flashlight::WGPUWrapper {
    class Device {
        WGPUDevice m_Device = nullptr;

    public:
        explicit Device(const Instance &instance);
        ~Device();

        Device(const Device &) = delete;
        Device(Device &&) = delete;

        Device &operator=(const Device &) = delete;
        Device &operator=(Device &&) = delete;

        [[nodiscard]] inline WGPUDevice GetDevice() const;

    private:
        void Create(WGPUInstance instance);
        void Destroy() const;

        static WGPUAdapter RequestAdapterSync(WGPUInstance instance, WGPURequestAdapterOptions const *options);
        static void InspectAdapter(WGPUAdapter adapter);
    };

#include "Device.inl"
}
