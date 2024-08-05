set_xmakever("2.9.3")

set_project("FlashlightEngine")
set_version("0.0.1")

set_allowedplats("windows", "linux")
set_allowedarchs("windows|x64", "linux|x64", "linux|x86_64")

add_rules("mode.debug", "mode.release")
set_languages("cxx20")

add_rules("plugin.vsxmake.autoupdate")

if (is_mode("release")) then
  set_optimize("fastest")
end

if (is_mode("debug")) then
    add_defines("FL_DEBUG")
end

add_repositories("pixfri https://github.com/Pixfri/xmake-repo.git")

-- Define packages to download.
add_requires("vulkan-loader 1.3.290+0", "vk-bootstrap v1.3.290", "vulkan-memory-allocator v3.1.0", 
             "vulkan-utility-libraries v1.3.290", "glfw 3.4", "glm 1.0.1", "spdlog v1.9.0", "magic_enum v0.9.5")
add_requires("glslang 1.3.290+0", {configs = {binaryonly = true}})
add_requires("fastgltf v0.6.1")
add_requires("imgui v1.91.0", {configs = {glfw = true, vulkan = true, debug = is_mode("debug")}})

local outputdir = "$(mode)-$(os)-$(arch)"

-- A rule to copy resources from a target's 'Resources' directory.
rule("cp-resources")
  after_build(function (target)
    os.cp(target:name() .. "/Resources", "build/" .. outputdir .. "/" .. target:name() .. "/bin")
  end)

target("FlashlightEngine")
  set_kind("binary")
  add_rules("cp-resources")
  add_rules("utils.glsl2spv", {outputdir = "build/" .. outputdir .. "/FlashlightEngine/bin/Shaders"})

  -- Set binary and object files directories.
  set_targetdir("build/" .. outputdir .. "/FlashlightEngine/bin")
  set_objectdir("build/" .. outputdir .. "/FlashlightEngine/obj")

  -- Set source cpp files.
  add_files("FlashlightEngine/Source/**.cpp")

  -- Add Engine headers to the project and set the include directory as public so it can be accessed from dependant
  -- targets.
  add_headerfiles("FlashlightEngine/Include/**.hpp", "FlashlightEngine/Include/**.h", "FlashlightEngine/Include/**.inl")
  add_includedirs("FlashlightEngine/Include", {public = true})
  add_headerfiles("FlashlightEngine/ThirdParty/**.h")
  add_includedirs("FlashlightEngine/ThirdParty", {public = true})
  
  add_files("FlashlightEngine/Shaders/**.vert", "FlashlightEngine/Shaders/**.frag", "FlashlightEngine/Shaders/**.comp") -- Tell glsl2spv to compile the files.
  add_headerfiles("FlashlightEngine/Shaders/**") -- A trick to make them show up in VS/Rider solutions.
  add_headerfiles("FlashlightEngine/Resources/**") -- A trick to make them show up in VS/Rider solutions.

  -- Precompiled header
  set_pcxxheader("FlashlightEngine/Include/FlashlightEngine/flpch.hpp")

  -- target dependencies
  add_packages("vulkan-loader","vk-bootstrap", "vulkan-memory-allocator", "vulkan-utility-libraries", "glfw", "glm",
               "spdlog", "magic_enum", "imgui", "fastgltf")