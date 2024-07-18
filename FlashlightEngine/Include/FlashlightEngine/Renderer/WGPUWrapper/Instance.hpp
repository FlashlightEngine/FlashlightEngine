/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Instance.hpp
 * Description : Declaration of the Instance class. The WebGPU instance is used to get the adapter.
 */
#pragma once

#include "FlashlightEngine/pch.hpp"

namespace Flashlight::WGPUWrapper {
    class Instance {
        WGPUInstance m_Instance = nullptr;

    public:
        inline Instance();
        inline ~Instance();

        Instance(const Instance&) = delete;
        Instance(Instance&&) = delete;

        Instance& operator=(const Instance&) = delete;
        Instance& operator=(Instance&&) = delete;

        [[nodiscard]] inline WGPUInstance GetNativeInstance() const;

    private:
        void Create();
        void Destroy() const;
    };

#include "Instance.inl"
}
