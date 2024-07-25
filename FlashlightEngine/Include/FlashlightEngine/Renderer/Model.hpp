/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Model.hpp
 * Description : Declaration of the Model class. It is used to represent a model. each model has its own vertices.
 */
#pragma once

#include "FlashlightEngine/pch.hpp"

#include <glm/glm.hpp>

#include <volk.h>

namespace Flashlight {
    struct Vertex {
        glm::vec2 Position;
        glm::vec3 Color;

        static inline VkVertexInputBindingDescription GetBindingDescription();
        static inline std::array<VkVertexInputAttributeDescription, 2> GetAttributesDescriptions();
    };

    class Model {
        std::vector<Vertex> m_Vertices;
        
    public:
        explicit inline Model(const std::vector<Vertex>& vertices);
        ~Model() = default;

        Model(const Model& rhs) = default;
        Model(Model&& rhs) = default;

        Model& operator=(const Model& rhs) = default;
        Model& operator=(Model&& rhs) = default;

        void Draw(VkCommandBuffer commandBuffer);

        [[nodiscard]] inline std::vector<Vertex> GetVertices() const;
    };

#include "Model.inl"
}
