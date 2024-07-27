#pragma once

#include "FlashlightEngine/Core/Window.hpp"

#include "FlashlightEngine/pch.hpp"

#include <d3d11.h>
#include <dxgi1_3.h>

namespace Flashlight {
    class Renderer {
        ComPtr<ID3D11Device> m_Device = nullptr;
        ComPtr<ID3D11DeviceContext> m_DeviceContext = nullptr;
        ComPtr<IDXGIFactory2> m_DxgiFactory = nullptr;
        ComPtr<IDXGISwapChain1> m_SwapChain = nullptr;
        ComPtr<ID3D11RenderTargetView> m_RenderTargetView = nullptr;
        
    public:
        explicit Renderer(const Window& window);
        ~Renderer();

        Renderer(const Renderer&) = delete;
        Renderer(Renderer&&) = delete;

        Renderer& operator=(const Renderer&) = delete;
        Renderer& operator=(Renderer&&) = delete;

        [[nodiscard]] inline ID3D11Device* GetDevice() const;
        [[nodiscard]] inline ID3D11DeviceContext* GetDeviceContext() const;
        [[nodiscard]] inline IDXGIFactory2* GetFactory() const;
        [[nodiscard]] inline IDXGISwapChain1* GetSwapChain() const;
        [[nodiscard]] inline ID3D11RenderTargetView* GetRenderTargetView() const;

        void RecreateSwapChain(Window& window);

        void BeginRendering(const Window& window);
        void EndRendering() const;
        
    private:
        void CreateDeviceResources(const Window& window);
        void DestroyDeviceResources();
        
        void CreateSwapChainResources();
        void DestroySwapChainResources();
    };

#include "Renderer.inl"
}
