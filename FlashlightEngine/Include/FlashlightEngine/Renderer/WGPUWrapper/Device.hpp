/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Device.hpp
 * Description : Declaration of the Device class, used to create other objects with WebGPU.
 */
#pragma once

#include "FlashlightEngine/Renderer/WGPUWrapper/Instance.hpp"

#include "FlashlightEngine/Core/Window.hpp"

#include "FlashlightEngine/pch.hpp"

namespace Flashlight::WGPUWrapper {
    class Device {
        WGPUDevice m_Device = nullptr;
        WGPUSurface m_Surface = nullptr;

    public:
        inline Device(const Instance& instance, const Window& window);
        inline ~Device();

        Device(const Device&) = default;
        Device(Device&&) = delete;

        Device& operator=(const Device&) = default;
        Device& operator=(Device&&) = delete;

        [[nodiscard]] inline WGPUDevice GetNativeDevice() const;
        [[nodiscard]] inline WGPUSurface GetSurface() const;

    private:
        void Create(WGPUInstance instance, GLFWwindow* window);
        void Destroy() const;

        static WGPUAdapter RequestAdapterSync(WGPUInstance instance, WGPURequestAdapterOptions const* options);
        static void InspectAdapter(WGPUAdapter adapter);
        static WGPUDevice RequestDeviceSync(WGPUAdapter adapter, WGPUDeviceDescriptor const* descriptor);
        static void InspectDevice(WGPUDevice device);
    };

#include "Device.inl"
}
