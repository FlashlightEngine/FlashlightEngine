#ifndef OPENGLRENDERER_GLLOADER_HPP
#define OPENGLRENDERER_GLLOADER_HPP

#include "Core/Window.hpp"

#include <glad/glad.h>

class GLLoader {
public:
    // This class shouldn't be initialized nor destroyed.
    GLLoader() = delete;
    ~GLLoader() = delete;

    static void LoadGLLoader(const WindowProperties &windowProperties);
    static void EnableGLFeature(GLenum feature);
    static void UpdateViewportSize(int width, int height);
};


#endif // OPENGLRENDERER_GLLOADER_HPP
