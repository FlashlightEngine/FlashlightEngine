// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of the "Flashlight Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef FL_RENDERER_HPP
#define FL_RENDERER_HPP

#include <FLUtils/Defines.hpp>
#include <Flashlight/Renderer/Export.hpp>

namespace Flashlight {
    class FL_RENDERER_API Renderer {
    public:
        Renderer() = default;
        ~Renderer() = default;

        Renderer(const Renderer&) = delete;
        Renderer(Renderer&&) = delete;

        Renderer& operator=(const Renderer&) = delete;
        Renderer& operator=(Renderer&&) = delete;

    private:
    };
}

#include <Flashlight/Renderer/Renderer.inl>

#endif // FL_RENDERER_HPP
