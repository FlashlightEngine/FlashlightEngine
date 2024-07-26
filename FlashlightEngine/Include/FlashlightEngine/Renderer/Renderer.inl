#pragma once

inline void Renderer::ClearColor(const float red, const float green, const float blue, const float alpha) {
    glClearColor(red, green, blue, alpha);
    glClear(GL_COLOR_BUFFER_BIT);
}
