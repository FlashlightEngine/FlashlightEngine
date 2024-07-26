/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Mesh.cpp
 * Description : Definitions of inline from the Mesh class.
 */
#include "FlashlightEngine/Renderer/Mesh.hpp"

namespace Flashlight {
    Mesh::Mesh(const std::vector<float>& vertices, const std::vector<u32>& indices)
        : m_Vertices(vertices), m_Indices(indices) {
        glGenBuffers(1, &m_VertexBufferObject);
        glGenBuffers(1, &m_IndexBufferObject);
        glGenVertexArrays(1, &m_VertexArrayObject);
        glBindVertexArray(m_VertexArrayObject);

        glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferObject);
        glBufferData(GL_ARRAY_BUFFER, m_Vertices.size() * sizeof(float), m_Vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBufferObject);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(u32), m_Indices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(nullptr));
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

}
