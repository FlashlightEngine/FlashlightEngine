/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Mesh.inl
 * Description : Definitions of inline methods from the Mesh class.
 */
#pragma once
template <typename VertexType>
Mesh<VertexType>::Mesh(const std::shared_ptr<VulkanWrapper::Device>& device,
                       const std::vector<VertexType>& vertices,
                       const std::vector<u32>& indices) : m_Vertices(vertices), m_Indices(indices),
                                                          m_VertexBuffer(
                                                              device, sizeof(VertexType) * vertices.size(),
                                                              VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                                                              VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                                              VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT),
                                                          m_IndexBuffer(
                                                              device, sizeof(uint32_t) * indices.size(),
                                                              VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                                                              VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                                                              VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT),
                                                          m_Device(device) {
    SendMeshData();
}


template <typename VertexType>
void Mesh<VertexType>::Draw(const VkCommandBuffer commandBuffer) const {
    constexpr VkDeviceSize offsets[] = {0};
    const VkBuffer vertexBuffer = m_VertexBuffer.GetNativeBuffer();
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexBuffer, offsets);
    vkCmdBindIndexBuffer(commandBuffer, m_IndexBuffer.GetNativeBuffer(), 0, VK_INDEX_TYPE_UINT32);

    vkCmdDrawIndexed(commandBuffer, static_cast<u32>(m_Indices.size()), 1, 0, 0, 0);
}

template <typename VertexType>
std::vector<VertexType> Mesh<VertexType>::GetVertices() const {
    return m_Vertices;
}

template <typename VertexType>
std::vector<u32> Mesh<VertexType>::GetIndices() const {
    return m_Indices;
}

template <typename VertexType>
void Mesh<VertexType>::SendMeshData() const {
    const VkDeviceSize vertexBufferSize = sizeof(m_Vertices[0]) * m_Vertices.size();
    const VkDeviceSize indexBufferSize = sizeof(m_Indices[0]) * m_Indices.size();

    VulkanWrapper::Buffer stagingBuffer{
        m_Device, vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    };

    stagingBuffer.SendData(vertexBufferSize, m_Vertices.data(), 0);

    m_Device->CopyBuffer(stagingBuffer.GetNativeBuffer(), m_VertexBuffer.GetNativeBuffer(), vertexBufferSize);

    stagingBuffer.SendData(indexBufferSize, m_Indices.data(), 0);

    m_Device->CopyBuffer(stagingBuffer.GetNativeBuffer(), m_IndexBuffer.GetNativeBuffer(), indexBufferSize);
}
