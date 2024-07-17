#pragma once

inline Instance::Instance() {
    Create();
}

inline Instance::~Instance() {
    Destroy();
}

inline WGPUInstance Instance::GetNativeInstance() const {
    return m_Instance;
}
