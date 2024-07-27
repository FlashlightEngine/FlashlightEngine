#include "FlashlightEngine/DX11Renderer/Renderer.hpp"

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "dxguid.lib")

namespace Flashlight {
    Renderer::Renderer(const Window& window) {
        CreateDeviceResources(window);
        CreateSwapChainResources();
    }

    Renderer::~Renderer() {
        m_DeviceContext->Flush();
        DestroySwapChainResources();
        DestroyDeviceResources();
    }

    void Renderer::RecreateSwapChain(Window& window) {
        m_DeviceContext->Flush();

        DestroySwapChainResources();

        if (FAILED(
            m_SwapChain->ResizeBuffers(0, window.GetWidth(), window.GetHeight(), DXGI_FORMAT_B8G8R8A8_UNORM, 0))) {
            Log::EngineFatal({0x02, 0x05}, "Failed to recreate DirectX 11 swap chain buffers.");
        }

        CreateSwapChainResources();

        window.SwapChainRecreated();
    }

    void Renderer::BeginRendering(const Window& window) {
        D3D11_VIEWPORT viewport;
        viewport.TopLeftX = 0;
        viewport.TopLeftY = 0;
        viewport.Width = static_cast<float>(window.GetWidth());
        viewport.Height = static_cast<float>(window.GetHeight());
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;

        constexpr float clearColor[] = { 0.1f, 0.1f, 0.1f, 1.0f };

        m_DeviceContext->ClearRenderTargetView(m_RenderTargetView.Get(), clearColor);
        m_DeviceContext->RSSetViewports(1, &viewport);
        m_DeviceContext->OMSetRenderTargets(1, m_RenderTargetView.GetAddressOf(), nullptr);
    }

    void Renderer::EndRendering() const {
        if (FAILED(m_SwapChain->Present(1, 0))) {
            Log::EngineError("Failed to present frame to DirectX 11 swap chain.");
        }
    }
    
    void Renderer::CreateDeviceResources(const Window& window) {
        Log::EngineTrace("Creating DXGI factory...");
        if (FAILED(CreateDXGIFactory1(IID_PPV_ARGS(&m_DxgiFactory)))) {
            Log::EngineFatal({0x02, 0x00}, "Failed to create DXGI factory.");
        }
        Log::EngineTrace("DXGI factory created.");
        
        constexpr D3D_FEATURE_LEVEL deviceFeatureLevel = D3D_FEATURE_LEVEL_11_0;
        UINT deviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

        Log::EngineTrace("Creating DirectX 11 device...");
        if (FAILED(
            D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, deviceFlags, &deviceFeatureLevel, 1,
                D3D11_SDK_VERSION, &m_Device, nullptr, &m_DeviceContext))) {
            Log::EngineFatal({0x02, 0x01}, "Failed to create DirectX 11 device.");
        }
        Log::EngineTrace("DirectX 11 device created.");

        DXGI_SWAP_CHAIN_DESC1 swapChainDescriptor = {};
        swapChainDescriptor.Width = window.GetWidth();
        swapChainDescriptor.Height = window.GetHeight();
        swapChainDescriptor.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        swapChainDescriptor.SampleDesc.Count = 1;
        swapChainDescriptor.SampleDesc.Quality = 0;
        swapChainDescriptor.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDescriptor.BufferCount = 2;
        swapChainDescriptor.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapChainDescriptor.Scaling = DXGI_SCALING_STRETCH;
        swapChainDescriptor.Flags = {};

        DXGI_SWAP_CHAIN_FULLSCREEN_DESC swapChainFullscreenDescriptor = {};
        swapChainFullscreenDescriptor.Windowed = true;

        Log::EngineTrace("Creating DXGI swap chain...");
        if (FAILED(
            m_DxgiFactory->CreateSwapChainForHwnd(m_Device.Get(), glfwGetWin32Window(window.GetGlfwWindow()),
                &swapChainDescriptor, &swapChainFullscreenDescriptor, nullptr, &m_SwapChain))) {
            Log::EngineFatal({0x02, 0x02}, "Failed to create DirectX 11 swap chain.");
        }
        Log::EngineTrace("DXGI swap chain created.");
    }

    void Renderer::DestroyDeviceResources() {
        Log::EngineTrace("Destroying DXGI swap chain...");
        m_SwapChain.Reset();
        Log::EngineTrace("DXGI swap chain destroyed.");

        Log::EngineTrace("Destroying DXGI factory...");
        m_DxgiFactory.Reset();
        Log::EngineTrace("DXGI factory destroyed.");

        Log::EngineTrace("Destroying DirectX 11 device context...");
        m_DeviceContext.Reset();
        Log::EngineTrace("DirectX 11 device context destroyed.");

        Log::EngineTrace("Destroying DirectX 11 device...");
        m_Device.Reset();
        Log::EngineTrace("DirectX 11 device destroyed.");
    }

    void Renderer::CreateSwapChainResources() {
        Log::EngineTrace("Retrieving DXGI swap chain back buffer...");
        
        ComPtr<ID3D11Texture2D> backBuffer = nullptr;
        if (FAILED(m_SwapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer)))) {
            Log::EngineFatal({0x02, 0x03}, "Failed to retrieve back buffer from the swap chain.");
        }
        Log::EngineTrace("DXGI swap chain back buffer retrieved.");

        Log::EngineTrace("Creating DirectX 11 render target view...");
        if (FAILED(m_Device->CreateRenderTargetView(backBuffer.Get(), nullptr, &m_RenderTargetView))) {
            Log::EngineFatal({0x02, 0x04}, "Failed to create DirectX 11 render target view.");
        }
        Log::EngineTrace("DirectX 11 render target view created.");
    }

    void Renderer::DestroySwapChainResources() {
        Log::EngineTrace("Destroying DirectX 11 swap chain render target view...");
        m_RenderTargetView.Reset();
        Log::EngineTrace("DirectX 11 swap chain render target view destroyed.");
    }
}
