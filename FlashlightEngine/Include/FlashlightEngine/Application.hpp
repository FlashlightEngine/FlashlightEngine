#pragma once

#include "FlashlightEngine/Renderer/Renderer.hpp"

#include "FlashlightEngine/pch.hpp"

namespace Flashlight {
    class Application {
    public:
        virtual ~Application() = default;

        void Run();

        virtual bool Init() = 0;
        virtual void Update() = 0;
        virtual void Render() = 0;
        virtual void Cleanup() = 0;

        [[nodiscard]] inline bool IsRunning() const;
        inline void Close();

    protected:
        bool m_IsRunning = false;
        std::unique_ptr<Renderer> m_Renderer;
    };

    std::unique_ptr<Application> CreateApp();

#include "Application.inl"
}
