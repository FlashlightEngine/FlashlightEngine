#pragma once

#include "Device.hpp"

namespace Flashlight::VulkanWrapper {
    class Buffer {
        VkBuffer m_Buffer = VK_NULL_HANDLE;
        VkDeviceMemory m_BufferMemory = VK_NULL_HANDLE;
        void* m_MappedMemory = nullptr;
        bool m_IsMemoryMapped = false;
        VkMemoryPropertyFlags m_MemoryPropertyFlags = 0;

        std::shared_ptr<Device> m_Device;

    public:
        Buffer(const std::shared_ptr<Device>& device, VkDeviceSize bufferSize, VkBufferUsageFlags usage,
               VkMemoryPropertyFlags properties);
        inline ~Buffer();

        Buffer(const Buffer&) = delete;
        Buffer(Buffer&& rhs) noexcept;

        Buffer& operator=(const Buffer&) = delete;
        Buffer& operator=(Buffer&& rhs) noexcept;

        void Map(VkDeviceSize offset, VkDeviceSize size);
        void SendData(VkDeviceSize size, const void* data) const;
        void Unmap();

        [[nodiscard]] inline VkBuffer GetNativeBuffer() const;
    };

#include "Buffer.inl"
}
