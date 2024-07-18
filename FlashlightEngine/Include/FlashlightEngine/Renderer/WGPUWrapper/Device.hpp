/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Device.hpp
 * Description : Declaration of the Device class, used to create other objects with WebGPU.
 */
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
        static WGPUDevice RequestDeviceSync(WGPUAdapter adapter, WGPUDeviceDescriptor const *descriptor);
        static void InspectDevice(WGPUDevice device);
    };

#include "Device.inl"
}
