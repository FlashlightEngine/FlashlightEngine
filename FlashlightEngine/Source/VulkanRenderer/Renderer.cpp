#include "VulkanRenderer/Renderer.hpp"

#include "VulkanRenderer/VulkanObjects/VulkanDevice.hpp"

namespace Flashlight {
Renderer::Renderer(const Window &window) : m_Window(window) {
    Init();
}

void Renderer::Init() {
    m_Instance = VulkanInstance::Create();
}

std::unique_ptr<Renderer> Renderer::Create(const Window& window) {
    return std::make_unique<Renderer>(window);
}
}