/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Application.hpp
 * Description : Boilerplate class to create an application using Flashlight Engine.
 */
#pragma once

#include <FlashlightEngine/Core/Window.hpp>

#include <FlashlightEngine/VulkanRenderer/VulkanRenderer.hpp>

namespace Flashlight {
    class Application {
    public:
        virtual ~Application() = default;

        void Run(const WindowProperties& windowProperties, const DebugLevel& debugLevel);

        [[nodiscard]] static inline Application& GetRunningInstance();

    protected:
        virtual bool Init(const WindowProperties& windowProperties, const DebugLevel& debugLevel);
        virtual void Update();
        virtual void Render();
        virtual void Cleanup();

        [[nodiscard]] inline bool IsRunning() const;
        inline void Close();

        bool m_IsRunning = false;
        float m_DeltaTime = 0.016f;
        std::unique_ptr<Window> m_Window;
        std::unique_ptr<VulkanRenderer::VulkanRenderer> m_Renderer;

    private:
        std::chrono::high_resolution_clock::time_point m_CurrentTime;
        static Application* m_SLoadedApplication;
    };

#include "Application.inl"
}
