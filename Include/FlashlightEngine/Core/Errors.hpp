// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in LICENSE

#pragma once

#include <FlashlightEngine/flpch.hpp>

namespace Flashlight {
    class Errors {
    public:
        Errors() = default;
        ~Errors() = default;
        
        Errors(const Errors&) = delete;
        Errors(Errors&&) = delete;
        
        Errors& operator=(const Errors&) = delete;
        Errors& operator=(Errors&&) = delete;

    private:
    };
}

#include <FlashlightEngine/Core/Errors.inl>
