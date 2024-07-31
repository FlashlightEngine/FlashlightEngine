/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Window.cpp
 * Description : Definitions of methods from the Window class.
 */
#include <FlashlightEngine/Core/Window.hpp>

#include <FlashlightEngine/Core/Logger.hpp>

#include <SDL.h>

namespace Flashlight {
    Window::Window(const WindowProperties& windowProperties) {
        if (SDL_Init(SDL_INIT_VIDEO) != 0) {
            Log::EngineFatal({0x01, 0x00}, "Failed to initialize SDL. Error : {}", SDL_GetError());
        }

        constexpr SDL_WindowFlags windowFlags = SDL_WINDOW_VULKAN;

        Log::EngineTrace("Creating window...");
        m_Window = SDL_CreateWindow(windowProperties.Title.c_str(), SDL_WINDOWPOS_UNDEFINED,
                                    SDL_WINDOWPOS_UNDEFINED, windowProperties.Width, windowProperties.Height,
                                    windowFlags);

        if (m_Window == nullptr) {
            Log::EngineFatal({0x01, 0x01}, "Failed to create SDL window.");
        }

        Log::EngineTrace("Window created.");

        m_Data.Width = windowProperties.Width;
        m_Data.Height = windowProperties.Height;
        m_Data.Title = windowProperties.Title;
    }

    Window::~Window() {
        if (m_Window != nullptr) {
            Log::EngineTrace("Destroying window.");
            SDL_DestroyWindow(m_Window);
        }

        SDL_Quit();
    }

    void Window::Update() {
        SDL_Event event;

        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                m_Data.ShouldClose = true;
            }

            if (event.type == SDL_WINDOWEVENT) {
                if (event.window.event == SDL_WINDOWEVENT_MINIMIZED) {
                    m_Data.StopRendering = true;
                }

                if (event.window.event == SDL_WINDOWEVENT_RESTORED) {
                    m_Data.StopRendering = false;
                }
            }
        }
    }
}
