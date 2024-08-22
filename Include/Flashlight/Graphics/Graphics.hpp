// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of the "Flashlight Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef FL_GRAPHICS_HPP
#define FL_GRAPHICS_HPP

#include <FLUtils/Defines.hpp>
#include <Flashlight/Graphics/Export.hpp>

namespace Flashlight {
    class FL_GRAPHICS_API Graphics {
    public:
        Graphics() = default;
        ~Graphics() = default;

        Graphics(const Graphics&) = delete;
        Graphics(Graphics&&) = delete;

        Graphics& operator=(const Graphics&) = delete;
        Graphics& operator=(Graphics&&) = delete;

    private:
    };
}

#include <Flashlight/Graphics/Graphics.inl>

#endif // FL_GRAPHICS_HPP
