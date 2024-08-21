// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in LICENSE

#pragma once

#ifndef FL_STATIC
    #ifdef _WIN32
        #define FL_EXPORT __declspec(dllexport)
        #define FL_IMPORT __declspec(dllimport)
    #else
        #define FL_EXPORT [[gnu::visibility("default")]]
        #define FL_IMPORT [[gnu::visibility("default")]]
    #endif
#else
    #define FL_EXPORT
    #define FL_IMPORT
#endif

#ifdef FL_BUILD
    #define FL_API FL_EXPORT
#else
    #define FL_API FL_IMPORT
#endif