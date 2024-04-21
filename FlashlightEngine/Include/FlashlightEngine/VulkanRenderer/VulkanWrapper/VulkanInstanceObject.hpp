/* Copyright (C) 2024 Jean "Pixfri" Letessier (jean.letessier@protonmail.com)
 * This file is part of "FlashLight Engine"
 * For conditions of distribution and use, see copyright notice in FlashLightEngine.hpp
 *
 * File : VulkanObject.hpp
 * Description : This header file contains the definition of the VulkanObject class, a boilerplate class for vulkan objects.
 */

#pragma once

#include "VulkanInstance.hpp"

namespace Flashlight {

namespace VulkanWrapper {

    template<typename ClassName, typename VkType, typename VkCreateInfoType>
    class VulkanInstanceObject {
    public:
        VulkanInstanceObject();
        VulkanInstanceObject(const VulkanInstanceObject &) = delete;
        VulkanInstanceObject(VulkanInstanceObject &&) = delete;
        virtual ~VulkanInstanceObject();
        
        bool IsValid() const noexcept;
        VkType GetHandle() const noexcept;

        void Create(VulkanInstance &instance, VkCreateInfoType &createInfo);
        void Destroy();
   
    protected:
        VulkanInstance m_Instance;
        VkType m_Handle;
    };
    

#include "VulkanInstanceObject.inl"
}

}
