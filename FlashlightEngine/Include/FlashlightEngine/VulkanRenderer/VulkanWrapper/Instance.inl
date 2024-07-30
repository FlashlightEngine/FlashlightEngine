/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Device.hpp
 * Description : Definitions of inline methods the Instance class.
 */
#pragma once

inline VkInstance Instance::GetInstance() const {
    return m_Instance;
}

inline vkb::Instance Instance::GetVkbInstance() const {
    return m_VkbInstance;
}
