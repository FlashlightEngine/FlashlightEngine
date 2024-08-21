-- Copyright (C) 2024 Jean "Pixfri" Letessier 
-- This file is part of Flashlight Engine.
-- For conditions of distribution and use, see copyright notice in LICENSE

set_xmakever("2.9.3")

set_project("FlashlightEngine")

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

add_defines("GLFW_INCLUDE_VULKAN", "VK_NO_PROTOTYPES")

add_repositories("FlashlightEngine https://github.com/Pixfri/xmake-repo.git")

-- Define packages to download.
add_requires("volk 1.3.290+0", "vk-bootstrap v1.3.290", "vulkan-memory-allocator v3.1.0", 
             "vulkan-utility-libraries v1.3.290", "glfw 3.4", "glm 1.0.1", "spdlog v1.9.0", "stb 2024.06.01")
add_requires("imgui v1.91.0", {configs = {glfw = true, vulkan = true, debug = is_mode("debug")}})
add_packages("flutils 1.1.0")

local outputdir = "$(mode)-$(os)-$(arch)"

option("static", {description = "Build the engine into a static library.", default = false})

add_includedirs("Include")
  
target("FlashlightEngine")
  if has_config("static") then
    set_kind("static")
    add_defines("FL_STATIC", {public = true})
  else
    set_kind("shared")
  end
  
  add_defines("FL_BUILD")

  -- Set binary and object files directories.
  set_targetdir("build/" .. outputdir .. "/FlashlightEngine/bin")
  set_objectdir("build/" .. outputdir .. "/FlashlightEngine/obj")

  -- Set source cpp files.
  add_files("Source/**.cpp")

  -- Add Engine headers to the project and set the include directory as public so it can be accessed from dependant
  -- targets.
  add_headerfiles("Include/**.hpp", "Include/**.h", "Include/**.inl")
  
  -- Precompiled header
  set_pcxxheader("Include/FlashlightEngine/flpch.hpp")

  -- target dependencies
  add_packages("volk","vk-bootstrap", "vulkan-memory-allocator", "vulkan-utility-libraries", "glfw", "glm",
               "spdlog", "imgui", "stb", "flutils")

includes("xmake/**.lua")