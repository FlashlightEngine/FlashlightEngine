// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef FL_RHI_INSTANCE_HPP
#define FL_RHI_INSTANCE_HPP

#include <FLUtils/Defines.hpp>
#include <Flashlight/Export.hpp>

namespace Flashlight {
    class FL_API Instance {
    public:
        Instance() = default;
        ~Instance() = default;

        Instance(const Instance&) = delete;
        Instance(Instance&&) = delete;

        Instance& operator=(const Instance&) = delete;
        Instance& operator=(Instance&&) = delete;

    private:
    };
}

#include <Flashlight/RHI/Instance.inl>

#endif // FL_RHI_INSTANCE_HPP
