/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : VulkanTypes.cpp
 * Description : Definitions of methods from VulkanTypes.hpp.
 */
#include <FlashlightEngine/VulkanRenderer/VulkanTypes.hpp>

namespace Flashlight::VulkanRenderer {
    void Node::RefreshTransform(const glm::mat4& parentMatrix) {
        WorldTransform = parentMatrix * LocalTransform;
        for (const auto& c : Children) {
            c->RefreshTransform(WorldTransform);
        }
    }

    void Node::Draw(const glm::mat4& topMatrix, DrawContext& context) {
        // Draw children.
        for (const auto& c : Children) {
            c->Draw(topMatrix, context);
        }
    }
}
