/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Vertex.hpp
 * Description : Declaration of different Vertex-representing structures.
 */
#pragma once

#include <glm/glm.hpp>

// I don't know why, but ReSharper thinks this include isn't used.
// ReSharper disable once CppUnusedIncludeDirective
#include <volk.h>

namespace Flashlight {
    using position2D = glm::vec2;
    using color = glm::vec3;

    struct Vertex2DColor {
        position2D Position;
        color Color;

        [[nodiscard]] static inline std::vector<VkVertexInputBindingDescription> GetBindingDescription();
        [[nodiscard]] static inline std::vector<VkVertexInputAttributeDescription> GetAttributesDescriptions();
    };

#include "Vertex.inl"
}
