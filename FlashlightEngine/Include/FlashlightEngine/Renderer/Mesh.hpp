/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Model.hpp
 * Description : Declaration of the Model class. It is used to represent a model. each model has its own vertices.
 */
#pragma once

#include "FlashlightEngine/Renderer/VulkanWrapper/Buffer.hpp"

#include "FlashlightEngine/pch.hpp"

#include <glm/glm.hpp>

#include <volk.h>

namespace Flashlight {
    struct Vertex {
        glm::vec2 Position;
        glm::vec3 Color;

        static inline std::vector<VkVertexInputBindingDescription> GetBindingDescription();
        static inline std::vector<VkVertexInputAttributeDescription> GetAttributesDescriptions();
    };

    class Mesh {
        std::vector<Vertex> m_Vertices;
        std::vector<u32> m_Indices;

        VulkanWrapper::Buffer m_VertexBuffer;
        VulkanWrapper::Buffer m_IndexBuffer;
        
        std::shared_ptr<VulkanWrapper::Device> m_Device;
        
    public:
        Mesh(const std::shared_ptr<VulkanWrapper::Device>& device, const std::vector<Vertex>& vertices,
             const std::vector<u32>& indices);
        ~Mesh() = default;

        Mesh(const Mesh& rhs) = default;
        Mesh(Mesh&& rhs) = default;

        Mesh& operator=(const Mesh& rhs) = default;
        Mesh& operator=(Mesh&& rhs) = default;
        
        void Draw(VkCommandBuffer commandBuffer) const;

        [[nodiscard]] inline std::vector<Vertex> GetVertices() const;
        [[nodiscard]] inline std::vector<u32> GetIndices() const;

    private:
        void SendMeshData() const;
    };

#include "Mesh.inl"
}
