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

        // Use the same vectors for all meshes so that the memory isn't reallocated too often.
        std::vector<u32> indices;
        std::vector<Vertex> vertices;

        for (fastgltf::Mesh& mesh : gltf.meshes) {
            MeshAsset newMesh;
            newMesh.Name = mesh.name;

            // Clear the mesh arrays for each mesh so that they don't get merged by error.
            indices.clear();
            vertices.clear();

            for (auto&& primitive : mesh.primitives) {
                GeoSurface newSurface;
                newSurface.StartIndex = static_cast<u32>(indices.size());
                newSurface.Count = static_cast<u32>(gltf.accessors[primitive.indicesAccessor.value()].count);

                size initialVertex = vertices.size();

                // Load indices
                {
                    fastgltf::Accessor& indexAccessor = gltf.accessors[primitive.indicesAccessor.value()];
                    indices.reserve(indices.size() + indexAccessor.count);

                    fastgltf::iterateAccessor<u32>(gltf, indexAccessor, [&](const u32 index) {
                        indices.push_back(index + static_cast<u32>(initialVertex));
                    });
                }

                // Load vertex positions.
                {
                    fastgltf::Accessor& posAccessor = gltf.accessors[primitive.findAttribute("POSITION")->second];
                    vertices.resize(vertices.size() + posAccessor.count);

                    fastgltf::iterateAccessorWithIndex<glm::vec3>(gltf, posAccessor,
                                                                  [&](const glm::vec3 pos, const size index) {
                                                                      Vertex newVertex;
                                                                      newVertex.Position = pos;
                                                                      newVertex.Normal = {1, 0, 0};
                                                                      newVertex.Color = glm::vec4(1.f);
                                                                      newVertex.UvX = 0;
                                                                      newVertex.UvY = 0;
                                                                      vertices[initialVertex + index] = newVertex;
                                                                  });
                }

                // Load vertex normals.
                auto normals = primitive.findAttribute("NORMAL");
                if (normals != primitive.attributes.end()) {
                    fastgltf::iterateAccessorWithIndex<glm::vec3>(gltf, gltf.accessors[normals->second],
                                                                  [&](const glm::vec3 normal, const size index) {
                                                                      vertices[initialVertex + index].Normal =
                                                                          normal;
                                                                  });
                }

                // Load UVs
                auto uv = primitive.findAttribute("TEXCOORD_0");
                if (uv != primitive.attributes.end()) {
                    fastgltf::iterateAccessorWithIndex<glm::vec2>(gltf, gltf.accessors[uv->second],
                                                                  [&](const glm::vec2 vec, const size index) {
                                                                      vertices[initialVertex + index].UvX = vec.x;
                                                                      vertices[initialVertex + index].UvY = vec.y;
                                                                  });
                }

                // Load vertex colors.
                auto colors = primitive.findAttribute("COLOR_0");
                if (colors != primitive.attributes.end()) {
                    fastgltf::iterateAccessorWithIndex<glm::vec4>(gltf, gltf.accessors[colors->second],
                                                                  [&](const glm::vec4 color, const size index) {
                                                                      vertices[initialVertex + index].Color = color;
                                                                  });
                }
                newMesh.Surfaces.push_back(newSurface);
            }

            // Display the vertex normals
            if (true) {
                for (Vertex& vertex : vertices) {
                    vertex.Color = glm::vec4(vertex.Normal, 0.5f);
                }
            }
            newMesh.MeshBuffers = renderer->UploadMesh(indices, vertices);

            meshes.emplace_back(std::make_shared<MeshAsset>(std::move(newMesh)));
        }

        for (auto& mesh : meshes) {
            renderer->PlanMeshDeletion(mesh->MeshBuffers);
        }

        return meshes;
    }
}
