/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Renderer.inl
 * Description : Definitions of inline methods from the Renderer class.
 */
#pragma once

inline void Renderer::ClearColor(const float red, const float green, const float blue, const float alpha) {
    glClearColor(red, green, blue, alpha);
    glClear(GL_COLOR_BUFFER_BIT);
}

inline void Renderer::SetPolygonMode(const GLenum faceMode, const GLenum mode) {
    glPolygonMode(faceMode, mode);
}


inline void Renderer::UseMainProgram() const {
    glUseProgram(m_ShaderProgram);
}
