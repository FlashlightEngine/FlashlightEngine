#include "FlashlightEngine/Renderer/Renderer.hpp"

namespace Flashlight {

Renderer::Renderer(Window &window) {
    // Initializing base vulkan objects.
    m_Instance.Create();

#ifdef FL_DEBUG
    m_DebugMessenger.Create(m_Instance);
#endif

    m_WindowSurface.Create(m_Instance, window);

    m_Device.Create(m_Instance, m_WindowSurface);

    m_SwapChain.Create(m_Device, window, m_WindowSurface);
}

}