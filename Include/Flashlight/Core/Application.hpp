// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of the "Flashlight Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef FL_CORE_APPLICATION_HPP
#define FL_CORE_APPLICATION_HPP

#include <FLUtils/Defines.hpp>
#include <Flashlight/Core/Export.hpp>

namespace Flashlight {
    class FL_CORE_API Application {
    public:
        Application() = default;
        ~Application() = default;

        Application(const Application&) = delete;
        Application(Application&&) = delete;

        Application& operator=(const Application&) = delete;
        Application& operator=(Application&&) = delete;

    private:
    };
}

#endif // FL_CORE_APPLICATION_HPP

#include <Flashlight/Core/Application.inl>
