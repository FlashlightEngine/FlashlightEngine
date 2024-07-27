#pragma once

inline ID3D11Device* Renderer::GetDevice() const {
    return m_Device.Get();
}

inline ID3D11DeviceContext* Renderer::GetDeviceContext() const {
    return m_DeviceContext.Get();
}

inline IDXGIFactory2* Renderer::GetFactory() const {
    return m_DxgiFactory.Get();
}

inline IDXGISwapChain1* Renderer::GetSwapChain() const {
    return m_SwapChain.Get();
}

inline ID3D11RenderTargetView* Renderer::GetRenderTargetView() const {
    return m_RenderTargetView.Get();
}
