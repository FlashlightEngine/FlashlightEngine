#include "FlashlightEngine/Renderer/VulkanWrapper/Buffer.hpp"

namespace Flashlight::VulkanWrapper {
    Buffer::Buffer(const Device& device, const VkDeviceSize bufferSize, const VkBufferUsageFlags usage,
                   const VkMemoryPropertyFlags properties) : m_Device(device) {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = bufferSize;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(m_Device.GetNativeDevice(), &bufferInfo, nullptr, &m_Buffer) != VK_SUCCESS) {
            Log::EngineError("Failed to create Vulkan buffer.");
        }

        VkMemoryRequirements memoryRequirements;
        vkGetBufferMemoryRequirements(m_Device.GetNativeDevice(), m_Buffer, &memoryRequirements);

        VkMemoryAllocateInfo allocateInfo{};
        allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocateInfo.allocationSize = memoryRequirements.size;
        allocateInfo.memoryTypeIndex = m_Device.FindMemoryType(memoryRequirements.memoryTypeBits, properties);

        if (vkAllocateMemory(m_Device.GetNativeDevice(), &allocateInfo, nullptr, &m_BufferMemory) != VK_SUCCESS) {
            Log::EngineError("Failed to allocate Vulkan buffer memory.");
        }

        vkBindBufferMemory(m_Device.GetNativeDevice(), m_Buffer, m_BufferMemory, 0);
    }

    void Buffer::Map(const VkDeviceSize size, const VkDeviceSize offset) {
        vkMapMemory(m_Device.GetNativeDevice(), m_BufferMemory, offset, size, 0, &m_MappedMemory);
    }
    
    void Buffer::SendData(const VkDeviceSize size, const void* data) const {
        memcpy(m_MappedMemory, data, size);
    }
    void Buffer::Unmap() const {
        vkUnmapMemory(m_Device.GetNativeDevice(), m_BufferMemory);
    }


}
