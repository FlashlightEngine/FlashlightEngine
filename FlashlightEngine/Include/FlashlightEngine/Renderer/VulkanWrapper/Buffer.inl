#pragma once

Buffer::~Buffer() {
    vkDeviceWaitIdle(m_Device.GetNativeDevice());
    
    if (m_Buffer) {
        vkDestroyBuffer(m_Device.GetNativeDevice(), m_Buffer, nullptr);
    }
    
    if (m_BufferMemory) {
        vkFreeMemory(m_Device.GetNativeDevice(), m_BufferMemory, nullptr);    
    }
}

inline Buffer::Buffer(Buffer&& rhs) noexcept : m_Device(rhs.m_Device) {
    m_Buffer = rhs.m_Buffer;
    m_BufferMemory = rhs.m_BufferMemory;
    m_MappedMemory = rhs.m_MappedMemory;

    rhs.m_Buffer = VK_NULL_HANDLE;
    rhs.m_BufferMemory = VK_NULL_HANDLE;
    rhs.m_MappedMemory = nullptr;
}

inline Buffer& Buffer::operator=(Buffer&& rhs) noexcept {
    m_Buffer = rhs.m_Buffer;
    m_BufferMemory = rhs.m_BufferMemory;
    m_MappedMemory = rhs.m_MappedMemory;

    rhs.m_Buffer = VK_NULL_HANDLE;
    rhs.m_BufferMemory = VK_NULL_HANDLE;
    rhs.m_MappedMemory = nullptr;

    return *this;
}


inline VkBuffer Buffer::GetNativeBuffer() const {
    return m_Buffer;
}

inline VkDeviceMemory Buffer::GetNativeBufferMemory() const {
    return m_BufferMemory;
}

inline void* Buffer::GetMappedMemory() const {
    return m_MappedMemory;
}
