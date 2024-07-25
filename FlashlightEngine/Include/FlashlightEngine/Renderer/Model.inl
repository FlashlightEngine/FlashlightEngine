/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Model.inl
 * Description : Definitions of inline methods from the Model class.
 */
#pragma once

inline VkVertexInputBindingDescription Vertex::GetBindingDescription() {
    VkVertexInputBindingDescription bindingDescription;
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(Vertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return bindingDescription;
}

inline std::array<VkVertexInputAttributeDescription, 2> Vertex::GetAttributesDescriptions() {
    std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(Vertex, Position);

    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(Vertex, Color);
    
    return attributeDescriptions;
}

inline Model::Model(const std::vector<Vertex>& vertices) : m_Vertices(vertices) {
}

inline std::vector<Vertex> Model::GetVertices() const {
    return m_Vertices;
}
