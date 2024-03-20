#pragma once

#include "Core/Window.hpp"
#include "VulkanObjects/VulkanDevice.hpp"

namespace Flashlight {

class Renderer {
public:
    Renderer(const Window &window);
    ~Renderer() = default;

    static std::unique_ptr<Renderer> Create(const Window& window);

private:
    Window m_Window;
    std::unique_ptr<VulkanDevice> m_Device;

    void Init();
};

}