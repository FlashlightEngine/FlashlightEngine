#pragma once

#include <FlashlightEngine/Application.hpp>

#include <FlashlightEngine/Renderer/Mesh.hpp>

class TestApplication final : public Flashlight::Application {
    std::unique_ptr<Flashlight::Mesh> m_TriangleMesh;
public:
    TestApplication() = default;
    ~TestApplication() override = default;

    TestApplication(const TestApplication&) = delete;
    TestApplication(TestApplication&&) = delete;

    TestApplication& operator=(const TestApplication&) = delete;
    TestApplication& operator=(TestApplication&&) = delete;

    bool Init() override;
    void Update() override;
    void Render() override;
    void Cleanup() override;

private:
    void ProcessInput();
};
