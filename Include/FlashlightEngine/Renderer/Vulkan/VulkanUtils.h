// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.h

#pragma once

#ifndef FL_RENDERER_VULKAN_VULKANUTILS_H
#define FL_RENDERER_VULKAN_VULKANUTILS_H

#include "FlashlightEngine/Prerequisites.h"
#include "FlashlightEngine/Export.h"

#include "FlashlightEngine/Renderer/Vulkan/VulkanTypes.inl"

/**
 * Returns the string representation of result.
 * @param result The result to get the string for.
 * @param getExtended Indicates whether to also return an extended result.
 * @returns The error code and/or extended error message in string form. Defaults to success for unknown result types. 
 */
const char* flVulkanResultToString(VkResult result, FlBool8 getExtended);

/**
 * Indicates if the passed result is a success or an error
 */
FlBool8 flVulkanResultIsSuccess(VkResult result);

#endif // FL_RENDERER_VULKAN_VULKANUTILS_H
