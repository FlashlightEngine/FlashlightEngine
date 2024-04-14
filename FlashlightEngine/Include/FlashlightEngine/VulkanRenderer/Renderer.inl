/* Copyright (C) 2024 Jean "Pixfri" Letessier (jean.letessier@protonmail.com)
 * This file is part of "FlashLight Engine"
 * For conditions of distribution and use, see copyright notice in FlashLightEngine.hpp
 *
 * File : Renderer.inl
 * Description : This file contains the definitions of inline methods from the Renderer class.
 */

#pragma once

// /// @ingroup VulkanRenderer
// /// @class Flashlight::Renderer
// /// @brief VulkanRenderer wrapper class for base Vulkan objects.
// 
// /// @brief Constructor for the VulkanBase class. Calls the function to initialize
// /// every Vulkan object.
// ///
// /// @param window The window of the application using the renderer.
// inline Renderer::Renderer(const Window &window) : m_Window(window), m_VulkanBase(m_Window) {
//     Init();
// }
// 
// /// @brief Creates a new Renderer object and returns a std::unique_pointer of it.
// ///
// /// @param window The window of the application using the renderer.
// ///
// /// @returns A std::unique_pointer to the newly created Renderer object.
// inline std::unique_ptr<Renderer> Renderer::Create(const Window& window) {
//     return std::make_unique<Renderer>(window);
// }