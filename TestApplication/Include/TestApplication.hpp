#pragma once

#include <FlashlightEngine/Application.hpp>

#include <FlashlightEngine/DX11Renderer/Mesh.hpp>

class TestApplication final : public Flashlight::Application {
    ComPtr<ID3D11Buffer> m_TriangleVertices = nullptr;
    
public:
    TestApplication() = default;
    ~TestApplication() override = default;

    TestApplication(const TestApplication&) = delete;
    TestApplication(TestApplication&&) = delete;

    TestApplication& operator=(const TestApplication&) = delete;
    TestApplication& operator=(TestApplication&&) = delete;

protected:
    bool Init(const Flashlight::WindowProperties& windowProperties) override;
    void Update() override;
    void Render() override;
    void Cleanup() override;
};
