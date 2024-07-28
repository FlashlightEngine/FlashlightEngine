#include "FlashlightEngine/Renderer/VulkanWrapper/Buffer.hpp"

namespace Flashlight::VulkanWrapper {
    Buffer::Buffer(const std::shared_ptr<Device>& device, const VkDeviceSize bufferSize,
                   const VkBufferUsageFlags usage,
                   const VkMemoryPropertyFlags properties) : m_MemoryPropertyFlags(properties), m_Device(device) {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = bufferSize;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(m_Device->GetNativeDevice(), &bufferInfo, nullptr, &m_Buffer) != VK_SUCCESS) {
            Log::EngineError("Failed to create Vulkan buffer.");
        }

        VkMemoryRequirements memoryRequirements;
        vkGetBufferMemoryRequirements(m_Device->GetNativeDevice(), m_Buffer, &memoryRequirements);

        VkMemoryAllocateInfo allocateInfo{};
        allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocateInfo.allocationSize = memoryRequirements.size;
        allocateInfo.memoryTypeIndex = m_Device->FindMemoryType(memoryRequirements.memoryTypeBits, properties);

        if (vkAllocateMemory(m_Device->GetNativeDevice(), &allocateInfo, nullptr, &m_BufferMemory) != VK_SUCCESS) {
            Log::EngineError("Failed to allocate Vulkan buffer memory.");
        }

        vkBindBufferMemory(m_Device->GetNativeDevice(), m_Buffer, m_BufferMemory, 0);
    }

    Buffer::Buffer(Buffer&& rhs) noexcept {
        m_Device = rhs.m_Device;
        m_Buffer = rhs.m_Buffer;
        m_BufferMemory = rhs.m_BufferMemory;
        m_MappedMemory = rhs.m_MappedMemory;

        rhs.m_Buffer = VK_NULL_HANDLE;
        rhs.m_BufferMemory = VK_NULL_HANDLE;
        rhs.m_MappedMemory = nullptr;
    }

    Buffer& Buffer::operator=(Buffer&& rhs) noexcept {
        m_Buffer = rhs.m_Buffer;
        m_BufferMemory = rhs.m_BufferMemory;
        m_MappedMemory = rhs.m_MappedMemory;

        rhs.m_Buffer = VK_NULL_HANDLE;
        rhs.m_BufferMemory = VK_NULL_HANDLE;
        rhs.m_MappedMemory = nullptr;

        return *this;
    }

    void Buffer::Map(const VkDeviceSize offset, const VkDeviceSize size) {
        if (m_IsMemoryMapped || !(m_MemoryPropertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) || !(
            m_MemoryPropertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) {
            Log::EngineWarn("Buffer is either already mapped or doesn't have the right properties to be mapped.");
            return;
        }
        vkMapMemory(m_Device->GetNativeDevice(), m_BufferMemory, offset, size, 0, &m_MappedMemory);
        m_IsMemoryMapped = true;
    }

    void Buffer::SendData(const VkDeviceSize size, const void* data) const {
        if (!m_IsMemoryMapped || !(m_MemoryPropertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) || !(
            m_MemoryPropertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) {
            
            Log::EngineWarn(
                "Cannot write to buffer if it is not mapped or if it doesn't have the right memory properties.");
            return;
        }

        memcpy(m_MappedMemory, data, size);
    }

    void Buffer::Unmap() {
        if (!m_IsMemoryMapped || !(m_MemoryPropertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) || !(
            m_MemoryPropertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) {
            Log::EngineWarn("Cannot unmap buffer memory if it is not mapped or doesn't have the right memory properties.");
            return;
        }

        vkUnmapMemory(m_Device->GetNativeDevice(), m_BufferMemory);
        m_IsMemoryMapped = false;
    }

}
