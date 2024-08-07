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
#include <FlashlightEngine/VulkanRenderer/VulkanUtils/VulkanBufferUtils.hpp>
#include <FlashlightEngine/VulkanRenderer/VulkanUtils/VulkanImageUtils.hpp>

#include <fastgltf/glm_element_traits.hpp>
#include <fastgltf/parser.hpp>
#include <fastgltf/tools.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include <magic_enum.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

namespace Flashlight::Renderer {
    VkFilter ExtractFilter(const fastgltf::Filter filter) {
        switch (filter) {
        // nearest samplers
        case fastgltf::Filter::Nearest:
        case fastgltf::Filter::NearestMipMapNearest:
        case fastgltf::Filter::NearestMipMapLinear:
            return VK_FILTER_NEAREST;

        // linear samplers
        case fastgltf::Filter::Linear:
        case fastgltf::Filter::LinearMipMapNearest:
        case fastgltf::Filter::LinearMipMapLinear:
            return VK_FILTER_LINEAR;
        }

        return VK_FILTER_LINEAR;
    }

    VkSamplerMipmapMode ExtractMipmapMode(const fastgltf::Filter filter) {
        switch (filter) {
        case fastgltf::Filter::NearestMipMapNearest:
        case fastgltf::Filter::LinearMipMapNearest:
            return VK_SAMPLER_MIPMAP_MODE_NEAREST;

        case fastgltf::Filter::NearestMipMapLinear:
        case fastgltf::Filter::LinearMipMapLinear:
        default:
            return VK_SAMPLER_MIPMAP_MODE_LINEAR;
        }
    }

    void LoadedGLTF::Draw(const glm::mat4& topMatrix, DrawContext& context) {
        // Create renderables from the scene nodes.
        for (auto& n : TopNodes) {
            n->Draw(topMatrix, context);
        }
    }

    void LoadedGLTF::ClearAll() {
        VkDevice device = LinkedRenderer->GetDevice().GetDevice();
        VmaAllocator allocator = LinkedRenderer->GetAllocator();

        DescriptorPool.DestroyPools(device);

        VulkanUtils::DestroyBuffer(allocator, MaterialDataBuffer);

        for (const auto& mesh : Meshes | std::views::values) {
            VulkanUtils::DestroyBuffer(allocator, mesh->MeshBuffers.IndexBuffer);
            VulkanUtils::DestroyBuffer(allocator, mesh->MeshBuffers.VertexBuffer);
        }

        for (auto& image : Images | std::views::values) {
            if (image.Image == LinkedRenderer->ErrorCheckerboardImage.Image) {
                // Don't destroy the default images.
                continue;
            }
            VulkanUtils::DestroyImage(allocator, device, image);
        }

        for (auto& sampler : Samplers) {
            vkDestroySampler(device, sampler, nullptr);
        }
    }

    std::optional<std::shared_ptr<LoadedGLTF>>
    LoadGLTF(VulkanRenderer* renderer, const std::filesystem::path& path) {
        Log::EngineTrace("Loading glTF: {}", path.string());

        auto scene = std::make_shared<LoadedGLTF>();
        scene->LinkedRenderer = renderer;
        LoadedGLTF& file = *scene;

        fastgltf::Parser parser{};

        constexpr auto gltfOptions = fastgltf::Options::DontRequireValidAssetMember | fastgltf::Options::AllowDouble
            | fastgltf::Options::LoadGLBBuffers | fastgltf::Options::LoadExternalBuffers;

        fastgltf::GltfDataBuffer data;
        data.loadFromFile(path);

        fastgltf::Asset gltf;

        auto type = fastgltf::determineGltfFileType(&data);
        if (type == fastgltf::GltfType::glTF) {
            auto load = parser.loadGLTF(&data, path.parent_path(), gltfOptions);
            if (load) {
                gltf = std::move(load.get());
            } else {
                Log::EngineError("Failed to load glTF: {}", fastgltf::to_underlying(load.error()));
                return {};
            }
        } else if (type == fastgltf::GltfType::GLB) {
            auto load = parser.loadBinaryGLTF(&data, path.parent_path(), gltfOptions);
            if (load) {
                gltf = std::move(load.get());
            } else {
                Log::EngineError("Failed to load glTF: {}", fastgltf::to_underlying(load.error()));
                return {};
            }
        } else {
            Log::EngineError("Failed to determine glTF type.");
            return {};
        }

        std::vector<DescriptorAllocatorGrowable::PoolSizeRatio> sizes = {
            {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 3},
            {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 3},
            {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1}
        };

        file.DescriptorPool.Initialize(renderer->GetDevice().GetDevice(), static_cast<u32>(gltf.materials.size()),
                                       sizes);

        // load samplers
        for (fastgltf::Sampler& sampler : gltf.samplers) {
            VkSamplerCreateInfo samplerInfo = {.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO, .pNext = nullptr};
            samplerInfo.maxLod = VK_LOD_CLAMP_NONE;
            samplerInfo.minLod = 0;

            samplerInfo.magFilter = ExtractFilter(sampler.magFilter.value_or(fastgltf::Filter::Nearest));
            samplerInfo.minFilter = ExtractFilter(sampler.minFilter.value_or(fastgltf::Filter::Nearest));

            samplerInfo.mipmapMode = ExtractMipmapMode(sampler.minFilter.value_or(fastgltf::Filter::Nearest));

            VkSampler newSampler;
            vkCreateSampler(renderer->GetDevice().GetDevice(), &samplerInfo, nullptr, &newSampler);

            file.Samplers.push_back(newSampler);
        }

        // Temporary arrays for all the objects to use while creating the GLTF data
        std::vector<std::shared_ptr<MeshAsset>> meshes;
        std::vector<std::shared_ptr<Node>> nodes;
        std::vector<AllocatedImage> images;
        std::vector<std::shared_ptr<GLTFMaterial>> materials;

        // Load all textures
        for (fastgltf::Image& image : gltf.images) {
            const auto img = LoadImage(renderer, gltf, image);

            if (img.has_value()) {
                images.push_back(*img);
                file.Images[image.name.c_str()] = *img;
            } else {
                // We failed to load, so let's give the slot the error checkerboard texture to not completely break
                // loading.
                images.push_back(renderer->ErrorCheckerboardImage);
                Log::EngineWarn("glTF failed to load texture named {}", image.name);
            }
        }

        // create buffer to hold the material data
        file.MaterialDataBuffer = VulkanUtils::CreateBuffer(renderer->GetAllocator(),
                                                            sizeof(GLTFMetallic_Roughness::MaterialConstants) * gltf
                                                                                                                .materials
                                                                                                                .size(),
                                                            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                                            VMA_MEMORY_USAGE_CPU_TO_GPU);

        int dataIndex = 0;
        auto sceneMaterialConstants = static_cast<GLTFMetallic_Roughness::MaterialConstants*>(file.
                                                                                              MaterialDataBuffer.
                                                                                              Info.pMappedData);

        for (fastgltf::Material& mat : gltf.materials) {
            auto newMat = std::make_shared<GLTFMaterial>();
            materials.push_back(newMat);
            file.Materials[mat.name.c_str()] = newMat;

            GLTFMetallic_Roughness::MaterialConstants constants;
            constants.ColorFactors.x = mat.pbrData.baseColorFactor[0];
            constants.ColorFactors.y = mat.pbrData.baseColorFactor[1];
            constants.ColorFactors.z = mat.pbrData.baseColorFactor[2];
            constants.ColorFactors.w = mat.pbrData.baseColorFactor[3];

            constants.MetalRoughFactors.x = mat.pbrData.metallicFactor;
            constants.MetalRoughFactors.y = mat.pbrData.roughnessFactor;
            // write material parameters to buffer
            sceneMaterialConstants[dataIndex] = constants;

            auto passType = MaterialPass::MainColor;
            if (mat.alphaMode == fastgltf::AlphaMode::Blend) {
                passType = MaterialPass::Transparent;
            }

            GLTFMetallic_Roughness::MaterialResources materialResources;
            // default the material textures
            materialResources.ColorImage = renderer->WhiteImage;
            materialResources.ColorSampler = renderer->DefaultSamplerLinear;
            materialResources.MetalRoughnessImage = renderer->WhiteImage;
            materialResources.MetalRoughnessSampler = renderer->DefaultSamplerLinear;

            // set the uniform buffer for the material data
            materialResources.DataBuffer = file.MaterialDataBuffer.Buffer;
            materialResources.DataBufferOffset = dataIndex * sizeof(GLTFMetallic_Roughness::MaterialConstants);
            // grab textures from gltf file
            if (mat.pbrData.baseColorTexture.has_value()) {
                size_t img = gltf.textures[mat.pbrData.baseColorTexture.value().textureIndex].imageIndex.value();
                size_t sampler = gltf.textures[mat.pbrData.baseColorTexture.value().textureIndex].samplerIndex.
                                                                                                  value();

                materialResources.ColorImage = images[img];
                materialResources.ColorSampler = file.Samplers[sampler];
            }
            // build material
            newMat->Data = renderer->MetalRoughMaterial.WriteMaterial(
                renderer->GetDevice().GetDevice(), passType, materialResources, file.DescriptorPool);

            dataIndex++;
        }

        // use the same vectors for all meshes so that the memory doesn't get reallocated too often
        std::vector<u32> indices;
        std::vector<Vertex> vertices;

        for (fastgltf::Mesh& mesh : gltf.meshes) {
            auto newMesh = std::make_shared<MeshAsset>();
            meshes.push_back(newMesh);
            file.Meshes[mesh.name.c_str()] = newMesh;
            newMesh->Name = mesh.name;

            // clear the mesh arrays each mesh, we don't want to merge them by error
            indices.clear();
            vertices.clear();

            for (auto&& primitive : mesh.primitives) {
                GeoSurface newSurface;
                newSurface.StartIndex = static_cast<u32>(indices.size());
                newSurface.Count = static_cast<u32>(gltf.accessors[primitive.indicesAccessor.value()].count);

                size_t initialVertex = vertices.size();

                // load indices
                {
                    fastgltf::Accessor& indexAccessor = gltf.accessors[primitive.indicesAccessor.value()];
                    indices.reserve(indices.size() + indexAccessor.count);

                    fastgltf::iterateAccessor<std::uint32_t>(gltf, indexAccessor,
                                                             [&](const std::uint32_t idx) {
                                                                 indices.push_back(idx + initialVertex);
                                                             });
                }

                // load vertex positions
                {
                    fastgltf::Accessor& posAccessor = gltf.accessors[primitive.findAttribute("POSITION")->second];
                    vertices.resize(vertices.size() + posAccessor.count);

                    fastgltf::iterateAccessorWithIndex<glm::vec3>(gltf, posAccessor,
                                                                  [&](const glm::vec3 v, const size_t index) {
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
                    fastgltf::iterateAccessorWithIndex<glm::vec3>(gltf, gltf.accessors[normals->second],
                                                                  [&](const glm::vec3 v, const size_t index) {
                                                                      vertices[initialVertex + index].Normal = v;
                                                                  });
                }

                // load UVs
                auto uv = primitive.findAttribute("TEXCOORD_0");
                if (uv != primitive.attributes.end()) {
                    fastgltf::iterateAccessorWithIndex<glm::vec2>(gltf, gltf.accessors[uv->second],
                                                                  [&](const glm::vec2 v, const size_t index) {
                                                                      vertices[initialVertex + index].UvX = v.x;
                                                                      vertices[initialVertex + index].UvY = v.y;
                                                                  });
                }

                // load vertex colors
                auto colors = primitive.findAttribute("COLOR_0");
                if (colors != primitive.attributes.end()) {
                    fastgltf::iterateAccessorWithIndex<glm::vec4>(gltf, gltf.accessors[colors->second],
                                                                  [&](const glm::vec4 v, const size_t index) {
                                                                      vertices[initialVertex + index].Color = v;
                                                                  });
                }

                if (primitive.materialIndex.has_value()) {
                    newSurface.Material = materials[primitive.materialIndex.value()];
                } else {
                    newSurface.Material = materials[0];
                }

                // Loop the vertices of this surface, find min/max bounds.
                glm::vec3 minPos = vertices[initialVertex].Position;
                glm::vec3 maxPos = vertices[initialVertex].Position;
                for (int i = initialVertex; i < vertices.size(); i++) {
                    minPos = glm::min(minPos, vertices[i].Position);
                    maxPos = glm::max(maxPos, vertices[i].Position);
                }

                newSurface.Bounds.Origin = (maxPos + minPos) / 2.f;
                newSurface.Bounds.Extents = (maxPos - minPos) / 2.f;
                newSurface.Bounds.SphereRadius = glm::length(newSurface.Bounds.Extents);
                
                newMesh->Surfaces.push_back(newSurface);
            }

            newMesh->MeshBuffers = renderer->UploadMesh(indices, vertices);
        }

        // load all nodes and their meshes
        for (fastgltf::Node& node : gltf.nodes) {
            std::shared_ptr<Node> newNode;

            // find if the node has a mesh, and if it does hook it to the mesh pointer and allocate it with the MeshNode class
            if (node.meshIndex.has_value()) {
                newNode = std::make_shared<MeshNode>();
                dynamic_cast<MeshNode*>(newNode.get())->Mesh = meshes[*node.meshIndex];
            } else {
                newNode = std::make_shared<Node>();
            }

            nodes.push_back(newNode);
            file.Nodes[node.name.c_str()];

            std::visit(fastgltf::visitor{
                           [&](const fastgltf::Node::TransformMatrix& matrix) {
                               memcpy(&newNode->LocalTransform, matrix.data(), sizeof(matrix));
                           },
                           [&](const fastgltf::Node::TRS& transform) {
                               const glm::vec3 translation(transform.translation[0], transform.translation[1],
                                                           transform.translation[2]);
                               const glm::quat rotation(transform.rotation[3], transform.rotation[0],
                                                        transform.rotation[1],
                                                        transform.rotation[2]);
                               const glm::vec3 scale(transform.scale[0], transform.scale[1], transform.scale[2]);

                               const glm::mat4 translationMatrix = glm::translate(glm::mat4(1.f), translation);
                               const glm::mat4 rotationMatrix = glm::toMat4(rotation);
                               const glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.f), scale);

                               newNode->LocalTransform = translationMatrix * rotationMatrix * scaleMatrix;
                           }
                       },
                       node.transform);
        }

        // run loop again to set up transform hierarchy
        for (size i = 0; i < gltf.nodes.size(); ++i) {
            fastgltf::Node& node = gltf.nodes[i];
            std::shared_ptr<Node>& sceneNode = nodes[i];

            for (auto& c : node.children) {
                sceneNode->Children.push_back(nodes[c]);
                nodes[c]->Parent = sceneNode;
            }
        }

        // find the top nodes, with no parents
        for (auto& node : nodes) {
            if (node->Parent.lock() == nullptr) {
                file.TopNodes.push_back(node);
                node->RefreshTransform(glm::mat4{1.f});
            }
        }
        return scene;
    }

    std::optional<AllocatedImage> LoadImage(VulkanRenderer* renderer, fastgltf::Asset& asset,
                                            fastgltf::Image& image) {
        AllocatedImage newImage{};

        i32 width, height, nrChannels;

        std::visit(
            fastgltf::visitor{
                [](auto& arg) {
                },
                [&](fastgltf::sources::URI& filePath) {
                    assert(filePath.fileByteOffset == 0); // We don't support offsets with stbi.
                    assert(filePath.uri.isLocalPath()); // We're only capable of loading
                    // local files.

                    const std::string path(filePath.uri.path().begin(),
                                           filePath.uri.path().end()); // Thanks C++.
                    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 4);
                    if (data) {
                        VkExtent3D imagesize;
                        imagesize.width = width;
                        imagesize.height = height;
                        imagesize.depth = 1;

                        newImage = VulkanUtils::CreateImage(renderer->GetAllocator(),
                                                            renderer->GetDevice().GetDevice(), renderer, data,
                                                            imagesize, VK_FORMAT_R8G8B8A8_UNORM,
                                                            VK_IMAGE_USAGE_SAMPLED_BIT, true);

                        stbi_image_free(data);
                    }
                },
                [&](fastgltf::sources::Vector& vector) {
                    unsigned char* data = stbi_load_from_memory(vector.bytes.data(),
                                                                static_cast<int>(vector.bytes.size()),
                                                                &width, &height, &nrChannels, 4);
                    if (data) {
                        VkExtent3D imagesize;
                        imagesize.width = width;
                        imagesize.height = height;
                        imagesize.depth = 1;

                        newImage = VulkanUtils::CreateImage(renderer->GetAllocator(),
                                                            renderer->GetDevice().GetDevice(), renderer, data,
                                                            imagesize, VK_FORMAT_R8G8B8A8_UNORM,
                                                            VK_IMAGE_USAGE_SAMPLED_BIT, true);

                        stbi_image_free(data);
                    }
                },
                [&](fastgltf::sources::BufferView& view) {
                    auto& bufferView = asset.bufferViews[view.bufferViewIndex];
                    auto& buffer = asset.buffers[bufferView.bufferIndex];

                    std::visit(fastgltf::visitor{
                                   // We only care about VectorWithMime here, because we
                                   // specify LoadExternalBuffers, meaning all buffers
                                   // are already loaded into a vector.
                                   [](auto& arg) {
                                   },
                                   [&](fastgltf::sources::Vector& vector) {
                                       unsigned char* data = stbi_load_from_memory(
                                           vector.bytes.data() + bufferView.byteOffset,
                                           static_cast<int>(bufferView.byteLength),
                                           &width, &height, &nrChannels, 4);
                                       if (data) {
                                           VkExtent3D imagesize;
                                           imagesize.width = width;
                                           imagesize.height = height;
                                           imagesize.depth = 1;

                                           newImage = VulkanUtils::CreateImage(
                                               renderer->GetAllocator(), renderer->GetDevice().GetDevice(),
                                               renderer, data, imagesize, VK_FORMAT_R8G8B8A8_UNORM,
                                               VK_IMAGE_USAGE_SAMPLED_BIT, true);

                                           stbi_image_free(data);
                                       }
                                   }
                               },
                               buffer.data);
                },
            },
            image.data);

        // If any of the attempts to load the data failed, we haven't written the image so handle is null.
        if (newImage.Image == VK_NULL_HANDLE) {
            return {};
        } else {
            return newImage;
        }
    }
}
