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
    class FlashlightEngine {
        std::chrono::high_resolution_clock::time_point m_CurrentTime;
        static FlashlightEngine* m_SLoadedApplication;
        
    public:
        FlashlightEngine() = default;
        ~FlashlightEngine() = default;

        FlashlightEngine(const FlashlightEngine&) noexcept = delete;
        FlashlightEngine(FlashlightEngine&&) noexcept = delete;

        FlashlightEngine& operator=(const FlashlightEngine&) noexcept = delete;
        FlashlightEngine& operator=(FlashlightEngine&&) noexcept = delete;
        
        void Run(const WindowProperties& windowProperties, const DebugLevel& debugLevel);

        [[nodiscard]] static inline FlashlightEngine& GetRunningInstance();

    protected:
        bool Init(const WindowProperties& windowProperties, const DebugLevel& debugLevel);
        void Update();
        void Render();
        void Cleanup();

        [[nodiscard]] inline bool IsRunning() const;
        inline void Close();

        bool m_IsRunning = false;

        f32 m_DeltaTime = 0.016f;
        
        std::unique_ptr<Window> m_Window;
        std::unique_ptr<VulkanRenderer::VulkanRenderer> m_Renderer;

        VulkanRenderer::Camera m_Camera;
        f32 m_CameraSpeed = 1.f;

    private:
        void CreateEditorUi();
    };

#include "FlashlightEngine.inl"
}
