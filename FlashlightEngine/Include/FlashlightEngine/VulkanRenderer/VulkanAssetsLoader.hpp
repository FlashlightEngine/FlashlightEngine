/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : VulkanAssetsLoader.hpp
 * Description : Contains functions to load glTF models.
 */
#pragma once

#include <FlashlightEngine/VulkanRenderer/VulkanTypes.hpp>

#include <filesystem>

namespace Flashlight::VulkanRenderer {
    struct GLTFMaterial {
        MaterialInstance Data;
    };
    
    struct GeoSurface {
        u32 StartIndex;
        u32 Count;
        std::shared_ptr<GLTFMaterial> Material;
    };

    struct MeshAsset {
        std::string Name;

        std::vector<GeoSurface> Surfaces;
        GPUMeshBuffers MeshBuffers;
    };

    class VulkanRenderer;

    std::optional<std::vector<std::shared_ptr<MeshAsset>>> LoadGltfMeshes(
        VulkanRenderer* renderer, const std::filesystem::path& filePath);
}
