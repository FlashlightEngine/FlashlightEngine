/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Queue.cpp
 * Description : Definitions of methods from the Queue class.
 */

#include "FlashlightEngine/Renderer/WGPUWrapper/Queue.hpp"

#include "FlashlightEngine/Core/Logger.hpp"

#include "FlashlightEngine/pch.hpp"

#include <magic_enum.hpp>

namespace Flashlight::WGPUWrapper {
    void Queue::Create(WGPUDevice device) {
        m_Queue = wgpuDeviceGetQueue(device);

        auto onQueueWorkDone = [](WGPUQueueWorkDoneStatus status, void* /*pUserData*/) {
            Log::EngineTrace("Queue work finished with status: {0}", magic_enum::enum_name<WGPUQueueWorkDoneStatus>(status));
        };
        wgpuQueueOnSubmittedWorkDone(m_Queue, onQueueWorkDone, nullptr);
    }

    void Queue::Destroy() const {
        if (m_Queue) {
            wgpuQueueRelease(m_Queue);
        }
    }
}
