/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Model.inl
 * Description : Definitions of inline methods from the Model class.
 */
#include "FlashlightEngine/Renderer/Mesh.hpp"

namespace Flashlight {
    Mesh::Mesh(const VulkanWrapper::Device& device, const std::vector<Vertex>& vertices)
    : m_Vertices(vertices), m_VertexBuffer(device, sizeof(m_Vertices[0]) * vertices.size(),
                                             VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                             VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT),
     m_Device(device) {
        SendMeshData();
    }
    
    void Mesh::Bind(const VkCommandBuffer commandBuffer) const {
        constexpr VkDeviceSize offsets[] = {0};
        const VkBuffer vertexBuffer = m_VertexBuffer.GetNativeBuffer();
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexBuffer, offsets);
    }


    void Mesh::Draw(const VkCommandBuffer commandBuffer) const {
        vkCmdDraw(commandBuffer, static_cast<u32>(m_Vertices.size()), 1, 0, 0);
    }

    void Mesh::SendMeshData() {
        const VkDeviceSize bufferSize = sizeof(m_Vertices[0]) * m_Vertices.size();

        VulkanWrapper::Buffer stagingBuffer{m_Device, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT};

        stagingBuffer.SendData(sizeof(m_Vertices[0]) * m_Vertices.size(), m_Vertices.data(), 0);

        m_Device.CopyBuffer(stagingBuffer.GetNativeBuffer(), m_VertexBuffer.GetNativeBuffer(), bufferSize);
    }
}
