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
#include <FlashlightEngine/VulkanRenderer/Camera.hpp>

namespace Flashlight {    
    class Application {
        std::chrono::high_resolution_clock::time_point m_CurrentTime;
        static Application* m_SLoadedApplication;
        
    public:
        Application(const WindowProperties& windowProperties, const DebugLevel& debugLevel);
        virtual ~Application();

        Application(const Application&) noexcept = delete;
        Application(Application&&) noexcept = delete;

        Application& operator=(const Application&) noexcept = delete;
        Application& operator=(Application&&) noexcept = delete;
        
        void Run();

        [[nodiscard]] static inline Application& GetRunningInstance();

    protected:
        virtual void OnUpdate() = 0;
        virtual void OnEvent(Event& event) = 0;
        virtual void OnRender() = 0;

        [[nodiscard]] inline bool IsRunning() const;
        inline void Close();

        bool m_IsRunning = false;

        f32 m_DeltaTime = 0.016f;

        EngineStats m_EngineStats{};
        
        std::unique_ptr<Window> m_Window;
        std::unique_ptr<Renderer::VulkanRenderer> m_Renderer;

    private:
        void OnWindowClose(Event& event);
    };

#include "Application.inl"
}
