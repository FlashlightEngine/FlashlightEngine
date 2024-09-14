// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef FL_CORE_APPLICATION_HPP
#define FL_CORE_APPLICATION_HPP

#include <FLUtils/Defines.hpp>
#include <Flashlight/Export.hpp>

namespace Flashlight {
    class FL_API Application {
    public:
        Application();
        ~Application();

        Application(const Application&) = delete;
        Application(Application&&) = delete;

        Application& operator=(const Application&) = delete;
        Application& operator=(Application&&) = delete;

        void Run();

    private:
    };
}

#include <Flashlight/Application.inl>

#endif // FL_CORE_APPLICATION_HPP
