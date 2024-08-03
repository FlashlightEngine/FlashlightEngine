/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : VulkanAssetsLoader.cpp
 * Description : Definitions of methods from VulkanAssetsLoader.hpp.
 */
#include <FlashlightEngine/VulkanRenderer/VulkanAssetsLoader.hpp>

#include <FlashlightEngine/VulkanRenderer/VulkanRenderer.hpp>
#include <FlashlightEngine/VulkanRenderer/VulkanInitializers.hpp>

#include <fastgltf/glm_element_traits.hpp>
#include <fastgltf/parser.hpp>
#include <fastgltf/tools.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include <magic_enum.hpp>

#include <stb_image/stb_image.h>

namespace Flashlight::VulkanRenderer {
    std::optional<std::vector<std::shared_ptr<MeshAsset>>> LoadGltfMeshes(
        VulkanRenderer* renderer, const std::filesystem::path& filePath) {
        Log::EngineTrace("Loading glTF: {}", filePath.string());

        fastgltf::GltfDataBuffer data;
        data.loadFromFile(filePath);

        constexpr auto gltfOptions = fastgltf::Options::LoadGLBBuffers | fastgltf::Options::LoadExternalBuffers;

        fastgltf::Asset gltf;
        fastgltf::Parser parser{};

        auto load = parser.loadBinaryGLTF(&data, filePath.parent_path(), gltfOptions);
        if (load) {
            gltf = std::move(load.get());
        } else {
            Log::EngineTrace("Failed to load glTF model: {}", fastgltf::to_underlying(load.error()));
            return {};
        }

        std::vector<std::shared_ptr<MeshAsset>> meshes;

        // use the same vectors for all meshes so that the memory doesn't reallocate as
        // often
        std::vector<uint32_t> indices;
        std::vector<Vertex> vertices;
        for (fastgltf::Mesh& mesh : gltf.meshes) {
            MeshAsset newMesh;

            newMesh.Name = mesh.name;

            // clear the mesh arrays each mesh, we don't want to merge them by error
            indices.clear();
            vertices.clear();

            for (auto&& primitive : mesh.primitives) {
                GeoSurface newSurface;
                newSurface.StartIndex = static_cast<u32>(indices.size());
                newSurface.Count = static_cast<u32>(gltf.accessors[primitive.indicesAccessor.value()].count);

                size_t initialVertex = vertices.size();

                // load indexes
                {
                    fastgltf::Accessor& indexAccessor = gltf.accessors[primitive.indicesAccessor.value()];
                    indices.reserve(indices.size() + indexAccessor.count);

                    fastgltf::iterateAccessor<std::uint32_t>(gltf, indexAccessor,
                                                             [&](u32 idx) {
                                                                 indices.push_back(idx + initialVertex);
                                                             });
                }

                // load vertex positions
                {
                    fastgltf::Accessor& posAccessor = gltf.accessors[primitive.findAttribute("POSITION")->second];
                    vertices.resize(vertices.size() + posAccessor.count);

                    fastgltf::iterateAccessorWithIndex<glm::vec3>(gltf, posAccessor,
                                                                  [&](glm::vec3 v, size_t index) {
                                                                      Vertex newVertex;
                                                                      newVertex.Position = v;
                                                                      newVertex.Normal = {1, 0, 0};
                                                                      newVertex.Color = glm::vec4{1.f};
                                                                      newVertex.UvX = 0;
                                                                      newVertex.UvY = 0;
                                                                      vertices[initialVertex + index] = newVertex;
                                                                  });
                }

                // load vertex normals
                auto normals = primitive.findAttribute("NORMAL");
                if (normals != primitive.attributes.end()) {
                    fastgltf::iterateAccessorWithIndex<glm::vec3>(gltf, gltf.accessors[(*normals).second],
                                                                  [&](glm::vec3 v, size_t index) {
                                                                      vertices[initialVertex + index].Normal = v;
                                                                  });
                }

                // load UVs
                auto uv = primitive.findAttribute("TEXCOORD_0");
                if (uv != primitive.attributes.end()) {
                    fastgltf::iterateAccessorWithIndex<glm::vec2>(gltf, gltf.accessors[(*uv).second],
                                                                  [&](glm::vec2 v, size_t index) {
                                                                      vertices[initialVertex + index].UvX = v.x;
                                                                      vertices[initialVertex + index].UvY = v.y;
                                                                  });
                }

                // load vertex colors
                auto colors = primitive.findAttribute("COLOR_0");
                if (colors != primitive.attributes.end()) {
                    fastgltf::iterateAccessorWithIndex<glm::vec4>(gltf, gltf.accessors[(*colors).second],
                                                                  [&](glm::vec4 v, size_t index) {
                                                                      vertices[initialVertex + index].Color = v;
                                                                  });
                }
                newMesh.Surfaces.push_back(newSurface);
            }

            // display the vertex normals
            constexpr bool overrideColors = true;
            if (overrideColors) {
                for (Vertex& vtx : vertices) {
                    vtx.Color = glm::vec4(vtx.Normal, 1.f);
                }
            }
            newMesh.MeshBuffers = renderer->UploadMesh(indices, vertices);

            renderer->PlanMeshDeletion(newMesh.MeshBuffers);

            meshes.emplace_back(std::make_shared<MeshAsset>(std::move(newMesh)));
        }

        return meshes;
    }
}
