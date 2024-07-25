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

        inline Buffer(Buffer&& rhs) noexcept;
        inline Buffer& operator=(Buffer&&) noexcept;

        void Map(VkDeviceSize size, VkDeviceSize offset);
        void SendData(VkDeviceSize size, const void* data) const;
        void Unmap() const;

        [[nodiscard]] inline VkBuffer GetNativeBuffer() const;
        [[nodiscard]] inline VkDeviceMemory GetNativeBufferMemory() const;
        [[nodiscard]] inline void* GetMappedMemory() const;
    };

#include "Buffer.inl"
}
