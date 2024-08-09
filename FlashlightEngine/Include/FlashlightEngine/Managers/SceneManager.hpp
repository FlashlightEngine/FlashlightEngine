#pragma once

#include <FlashlightEngine/VulkanRenderer/VulkanTypes.hpp>
#include <FlashlightEngine/VulkanRenderer/VulkanAssetsLoader.hpp>
#include <FlashlightEngine/VulkanRenderer/VulkanRenderer.hpp>

namespace Flashlight {
    struct Scene {
        Renderer::DrawContext DrawContext;
        Renderer::GPUSceneData SceneData;
        std::shared_ptr<Renderer::LoadedGLTF> SceneModel;
    };

    class SceneManager {
        std::unordered_map<std::string, Scene> m_LoadedScenes;
        Renderer::VulkanRenderer* m_LinkedRenderer;

    public:
        SceneManager(Renderer::VulkanRenderer* renderer);
        ~SceneManager();

        SceneManager(const SceneManager&) = delete;
        SceneManager(SceneManager&&) = delete;

        SceneManager& operator=(const SceneManager&) = delete;
        SceneManager& operator=(SceneManager&&) = delete;

        void LoadScene(const std::filesystem::path& path, const std::string& name);
        void UnloadScene(const std::string& name);

        void UpdateScene(const glm::mat4& viewMatrix, f32 fov, const Window& window, const std::string& name);
        void RenderScene(const std::string& name, EngineStats& stats);

        void ClearScenes();
    };

#include <FlashlightEngine/Managers/SceneManager.inl>


}
