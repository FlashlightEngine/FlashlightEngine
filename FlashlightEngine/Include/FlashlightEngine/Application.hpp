/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Application.hpp
 * Description : Boilerplate class to create an application using Flashlight Engine.
 */
#pragma once

#include "FlashlightEngine/Core/Window.hpp"
#include "FlashlightEngine/DX11Renderer/Renderer.hpp"

#include "FlashlightEngine/pch.hpp"

namespace Flashlight {
    class Application {
    public:
        virtual ~Application() = default;

        void Run(const WindowProperties &windowProperties);

    protected:
        virtual bool Init(const WindowProperties& windowProperties);
        virtual void Update();
        virtual void Render() = 0;
        virtual void Cleanup() = 0;

        [[nodiscard]] inline bool IsRunning() const;
        inline void Close();

        bool m_IsRunning = false;
        std::unique_ptr<Window> m_Window;
        std::unique_ptr<Renderer> m_Renderer;
    };

    std::unique_ptr<Application> CreateApp();

#include "Application.inl"
}
