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

    template<typename VkType, typename VkCreateInfoType>
    class VulkanInstanceObject {
    public:
        explicit VulkanInstanceObject(const VulkanBase &base);
        VulkanInstanceObject(const VulkanInstanceObject &) = delete;
        VulkanInstanceObject(VulkanInstanceObject &&) = delete;
        ~VulkanInstanceObject();

        inline bool IsValid() const noexcept;

        void Create(VkCreateInfoType createInfo);
   
    protected:
        VkInstance m_Instance;
        VkType m_Handle;
        
        virtual void Init(VkCreateInfoType createInfo) = 0;
        virtual void Cleanup() const = 0;
    };

#include "VulkanInstanceObject.inl"
    
}

}
