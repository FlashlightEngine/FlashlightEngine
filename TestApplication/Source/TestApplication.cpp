#include "TestApplication.hpp"


bool TestApplication::Init() {
    std::vector<float> vertices {
         0.5f,  0.5f, 0.0f, // top right
         0.5f, -0.5f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f // top left
    };

    std::vector<u32> indices {
        0, 1, 3,
        1, 2, 3
    };
    
    m_TriangleMesh = std::make_unique<Flashlight::Mesh>(vertices, indices);
    m_IsRunning = true;

    return true;
}

void TestApplication::Update() {
    ProcessInput();
}

void TestApplication::Render() {
    m_Renderer->ClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    m_Renderer->UseMainProgram();
    m_TriangleMesh->Draw();
}

void TestApplication::Cleanup() {
}

void TestApplication::ProcessInput() {
    if (m_Window->GetKeyState(Flashlight::Keys::Escape) == Flashlight::KeyState::Pressed) {
        Close();
    }

    if (m_Window->GetKeyState(Flashlight::Keys::F1) == Flashlight::KeyState::Pressed) {
        m_Renderer->SetPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    if (m_Window->GetKeyState(Flashlight::Keys::F1) == Flashlight::KeyState::Released) {
        m_Renderer->SetPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}
