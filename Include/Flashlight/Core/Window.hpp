// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef FL_CORE_WINDOW_HPP
#define FL_CORE_WINDOW_HPP

#include <FLUtils/Defines.hpp>
#include <Flashlight/Export.hpp>

namespace Flashlight {
    class FL_API Window {
    public:
        Window() = default;
        ~Window() = default;

        Window(const Window&) = delete;
        Window(Window&&) = delete;

        Window& operator=(const Window&) = delete;
        Window& operator=(Window&&) = delete;

    private:
    };
}

#include <Flashlight/Core/Window.inl>

#endif // FL_CORE_WINDOW_HPP
