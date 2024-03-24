/* Copyright (C) 2024 Jean "Pixfri" Letessier (jean.letessier@protonmail.com)
 * This file is part of "FlashLight Engine"
 * For conditions of distribution and use, see copyright notice in FlashLightEngine.hpp
 *
 * pch.hpp - Precompiled Header
 * This header file is precompiled to speed up build speeds. It should only contain
 * headers from the standard library, headers from external libraries or from the engine
 * that are used a lot and that are not often modified.
 *
 */

#pragma once

#include "defines.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <cstring>
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>