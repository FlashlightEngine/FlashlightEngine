// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of the "Flashlight Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef FL_CORE_HPP
#define FL_CORE_HPP

#include <FLUtils/Defines.hpp>
#include <Flashlight/Core/Export.hpp>

namespace Flashlight {
    class FL_CORE_API Core {
    public:
        Core() = default;
        ~Core() = default;

        Core(const Core&) = delete;
        Core(Core&&) = delete;

        Core& operator=(const Core&) = delete;
        Core& operator=(Core&&) = delete;

    private:
    };
}

#include <Flashlight/Core/Core.inl>

#endif // FL_CORE_HPP
