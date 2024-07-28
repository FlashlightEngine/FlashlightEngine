#pragma once

#include <FlashlightEngine/Application.hpp>

#include <FlashlightEngine/Renderer/RendererStructures/Mesh.hpp>
#include <FlashlightEngine/Renderer/RendererStructures/Vertex.hpp>

class TestApplication final : public Flashlight::Application {
    std::unique_ptr<Flashlight::Mesh<Flashlight::Vertex2DColor>> m_RectangleMesh;
public:
    TestApplication() = default;
    ~TestApplication() override = default;

    TestApplication(const TestApplication&) = delete;
    TestApplication(TestApplication&&) = delete;

    TestApplication& operator=(const TestApplication&) = delete;
    TestApplication& operator=(TestApplication&&) = delete;

protected:
    bool Init(const Flashlight::WindowProperties& windowProperties, const Flashlight::DebugLevel& debugLevel) override;
    void Update() override;
    void Render() override;
    void Cleanup() override;
};
