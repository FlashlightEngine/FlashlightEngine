// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of the "Flashlight Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef FL_PLATFORM_HPP
#define FL_PLATFORM_HPP

#include <FLUtils/Defines.hpp>
#include <Flashlight/Platform/Export.hpp>

namespace Flashlight {
    class FL_PLATFORM_API Platform {
    public:
        Platform() = default;
        ~Platform() = default;

        Platform(const Platform&) = delete;
        Platform(Platform&&) = delete;

        Platform& operator=(const Platform&) = delete;
        Platform& operator=(Platform&&) = delete;

    private:
    };
}

#include <Flashlight/Platform/Platform.inl>

#endif // FL_PLATFORM_HPP
