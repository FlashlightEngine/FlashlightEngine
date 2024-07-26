/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Mesh.inl
 * Description : Definitions of inline methods from the Mesh class.
 */
#pragma once

inline void Mesh::Draw() const {
    glBindVertexArray(m_VertexArrayObject);
    glDrawElements(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, nullptr);
}
