/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Mesh.hpp
 * Description : Declaration of the Mesh class. The Mesh class is used to store a mesh's vertices, indices and its
 *               buffers.
 */
#pragma once

#include "FlashlightEngine/pch.hpp"

#include <glad/glad.h>
#include <glm/glm.hpp>

namespace Flashlight {
    
    class Mesh {
        u32 m_VertexArrayObject;
        u32 m_VertexBufferObject;
        u32 m_IndexBufferObject;

        std::vector<float> m_Vertices;
        std::vector<u32> m_Indices;
    public:
        explicit Mesh(const std::vector<float>& vertices, const std::vector<u32>& indices);
        ~Mesh() = default;

        inline void Draw() const;
    };

#include "Mesh.inl"
}
