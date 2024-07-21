/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Application.hpp
 * Description : Boilerplate class to create an application using Flashlight Engine.
 */
#pragma once

#include "FlashlightEngine/Core/Window.hpp"

#include "FlashlightEngine/Renderer/Renderer.hpp"

#include "FlashlightEngine/pch.hpp"

namespace Flashlight {
    class Application {
    public:
        virtual ~Application() = default;

        void Run(const WindowProperties &windowProperties);

        virtual bool Init() = 0;
        virtual void Update() = 0;
        virtual void Render() = 0;
        virtual void Cleanup() = 0;

        [[nodiscard]] inline bool IsRunning() const;
        inline void Close();

    protected:
        bool m_IsRunning = false;
        std::unique_ptr<Window>  m_Window;
        std::unique_ptr<Renderer> m_Renderer;
    };

    std::unique_ptr<Application> CreateApp();

#include "Application.inl"
}
