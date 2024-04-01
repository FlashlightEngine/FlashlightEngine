/* Copyright (C) 2024 Jean "Pixfri" Letessier (jean.letessier@protonmail.com)
 * This file is part of "FlashLight Engine"
 * For conditions of distribution and use, see copyright notice in FlashLightEngine.hpp
 *
 * File : VulkanObject.hpp
 * Description : This header file contains the definition of the VulkanObject class, a boilerplate class for vulkan objects.
 */

#pragma once

#include "VulkanBase.hpp"

namespace Flashlight {

namespace VulkanWrapper {

    template<typename VkType>
    class VulkanDeviceObject {
    public:
        explicit VulkanDeviceObject(const VulkanBase &base);
        VulkanDeviceObject(const VulkanDeviceObject &) = delete;
        VulkanDeviceObject(VulkanDeviceObject &&) = delete;
        ~VulkanDeviceObject();

        inline bool IsValid() const noexcept;
   
    protected:
        VkDevice m_Device;
        VkType m_Handle;
        
        virtual void Init() = 0;
        virtual void Cleanup() const = 0;
    };

#include "VulkanDeviceObject.inl"
    
}

}
