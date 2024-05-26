/* Copyright (C) 2024 Jean "Pixfri" Letessier (jean.letessier@protonmail.com)
* This file is part of "FlashLight Engine"
 * For conditions of distribution and use, see copyright notice in FlashLightEngine.hpp
 *
 * File : VulkanRenderer.hpp
 * Description : This file includes the header files contained in the "VulkanRenderer" module the user may want to use.
 */

#pragma once

#include "VulkanInstance.hpp"

#ifdef FL_DEBUG
#include "VulkanDebugMessenger.hpp"
#endif

#include "VulkanDebugMessenger.hpp"
#include "VulkanWindowSurface.hpp"
#include "VulkanDevice.hpp"
#include "VulkanSwapChain.hpp"
#include "VulkanImageView.hpp"
#include "VulkanPipelineLayout.hpp"
#include "VulkanGraphicsPipeline.hpp"
#include "VulkanShaderModule.hpp"
#include "VulkanRenderPass.hpp"