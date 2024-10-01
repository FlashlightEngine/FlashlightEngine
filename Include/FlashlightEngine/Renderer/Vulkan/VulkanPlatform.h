// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.h

#pragma once

#ifndef FL_RENDERER_VULKAN_VULKANPLATFORM_H
#define FL_RENDERER_VULKAN_VULKANPLATFORM_H

#include "FlashlightEngine/Prerequisites.h"
#include "FlashlightEngine/Export.h"

/**
 * Appends the names of required extensions for this platform to
 * the namesDArray, which should be created and passed in.
 */
void flPlatformGetRequiredExtensionNames(const char*** namesDArray);

#endif // FL_RENDERER_VULKAN_VULKANPLATFORM_H
