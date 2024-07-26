#pragma once

#include <glad/glad.h>

namespace Flashlight {
    class Renderer {
    public:
        Renderer() = default;
        ~Renderer() = default;

        Renderer(const Renderer&) = default;
        Renderer(Renderer&&) = default;

        Renderer& operator=(const Renderer&) = default;
        Renderer& operator=(Renderer&&) = default;

        static inline void ClearColor(float red, float green, float blue, float alpha);
    };

#include "Renderer.inl"
}
