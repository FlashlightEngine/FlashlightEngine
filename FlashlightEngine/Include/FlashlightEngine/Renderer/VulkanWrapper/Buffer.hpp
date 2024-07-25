#pragma once

#include "Device.hpp"

namespace Flashlight::VulkanWrapper {
    class Buffer {
        VkBuffer m_Buffer = VK_NULL_HANDLE;
        VkDeviceMemory m_BufferMemory = VK_NULL_HANDLE;
        void* m_MappedMemory = nullptr;

        const Device& m_Device;

    public:
        Buffer(const Device& device, VkDeviceSize bufferSize, VkBufferUsageFlags usage,
               VkMemoryPropertyFlags properties);
        inline ~Buffer();

        void SendData(VkDeviceSize size, const void* data, VkDeviceSize offset);

        [[nodiscard]] inline VkBuffer GetNativeBuffer() const;
    };

#include "Buffer.inl"
}
