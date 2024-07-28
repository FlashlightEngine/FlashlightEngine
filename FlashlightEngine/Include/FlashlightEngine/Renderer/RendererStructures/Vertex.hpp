#pragma once

#include <glm/glm.hpp>

#include <volk.h>

namespace Flashlight {
    using position2D = glm::vec2;
    using color = glm::vec3;

    struct Vertex2DColor {
        position2D Position;
        color Color;

        static std::vector<VkVertexInputBindingDescription> GetBindingDescription();
        static std::vector<VkVertexInputAttributeDescription> GetAttributesDescriptions();
    };
}
