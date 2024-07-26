/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Renderer.hpp
 * Description : Declaration of the Renderer class. The Renderer class is used to store shader programs and set
 *               OpenGL states.
 */
#pragma once

#include <glad/glad.h>

namespace Flashlight {
    class Renderer {
        u32 m_ShaderProgram;
        
    public:
        Renderer();
        ~Renderer() = default;

        Renderer(const Renderer&) = default;
        Renderer(Renderer&&) = default;

        Renderer& operator=(const Renderer&) = default;
        Renderer& operator=(Renderer&&) = default;
        
        static inline void ClearColor(float red, float green, float blue, float alpha);
        static inline void SetPolygonMode(GLenum faceMode, GLenum mode);
        inline void UseMainProgram() const;
    };

#include "Renderer.inl"
}
