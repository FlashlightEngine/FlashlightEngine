/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : VulkanAssetsLoader.hpp
 * Description : Contains functions to load glTF models.
 */
#pragma once

#include <FlashlightEngine/VulkanRenderer/VulkanTypes.hpp>
#include <FlashlightEngine/VulkanRenderer/VulkanDescriptors.hpp>

#include <fastgltf/parser.hpp>

#include <filesystem>

namespace Flashlight::Renderer {
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

    struct LoadedGLTF : IRenderable {
        // storage for all the data of a given glTF file.
        std::unordered_map<std::string, std::shared_ptr<MeshAsset>> Meshes;
        std::unordered_map<std::string, std::shared_ptr<Node>> Nodes;
        std::unordered_map<std::string, AllocatedImage> Images;
        std::unordered_map<std::string, std::shared_ptr<GLTFMaterial>> Materials;

        // Nodes that don't have a parent, for iterating through the file in tree order.
        std::vector<std::shared_ptr<Node>> TopNodes;

        std::vector<VkSampler> Samplers;

        DescriptorAllocatorGrowable DescriptorPool;

        AllocatedBuffer MaterialDataBuffer;

        VulkanRenderer* LinkedRenderer;

        ~LoadedGLTF() {
            ClearAll();
        }

        virtual void Draw(const glm::mat4& topMatrix, DrawContext& context) override;

    private:
        void ClearAll();
    };

    std::optional<std::shared_ptr<LoadedGLTF>>
    LoadGLTF(VulkanRenderer* renderer, const std::filesystem::path& path);
    std::optional<AllocatedImage> LoadImage(VulkanRenderer* renderer, fastgltf::Asset& asset,
                                            fastgltf::Image& image);
}
