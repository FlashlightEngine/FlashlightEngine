/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : VulkanTypes.hpp
 * Description : Header which includes different Vulkan headers used across the whole renderer, as well as a macro
 *               to check the return status of vulkan functions.
 */
#pragma once

#include <FlashlightEngine/pch.hpp>
#include <FlashlightEngine/Core/Logger.hpp>

#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>
#include <vk_mem_alloc.h>

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

#define VK_CHECK(x)                                                                 \
    do {                                                                            \
        VkResult err = x;                                                           \
        if (err) {                                                                  \
            Flashlight::Log::EngineError("Vulkan Error: {}", string_VkResult(err)); \
            abort();                                                                \
        }                                                                           \
    } while (0);
