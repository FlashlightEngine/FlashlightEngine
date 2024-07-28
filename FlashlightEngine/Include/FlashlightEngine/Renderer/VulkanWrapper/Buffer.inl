#pragma once

Buffer::~Buffer() {
    vkDeviceWaitIdle(m_Device->GetNativeDevice());

    if (m_IsMemoryMapped) {
        Unmap();
    }
    
    if (m_Buffer) {
        vkDestroyBuffer(m_Device->GetNativeDevice(), m_Buffer, nullptr);
    }
    
    if (m_BufferMemory) {
        vkFreeMemory(m_Device->GetNativeDevice(), m_BufferMemory, nullptr);    
    }
}

inline VkBuffer Buffer::GetNativeBuffer() const {
    return m_Buffer;
}

inline VkDescriptorBufferInfo Buffer::GetDescriptorInfo(const VkDeviceSize range, const VkDeviceSize offset) const {
    return {m_Buffer, offset, range};
}