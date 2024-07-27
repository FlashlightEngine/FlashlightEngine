set_xmakever("2.9.3")

set_project("FlashlightEngine")
set_version("0.0.1")

set_allowedplats("windows")
set_allowedarchs("windows|x64")

add_rules("mode.debug", "mode.release")
set_languages("cxx20")

add_rules("plugin.vsxmake.autoupdate")

if (is_mode("release")) then
  set_optimize("fastest")
end

if (is_mode("debug")) then
    add_defines("FL_DEBUG")
end

-- Define packages to download.
add_requires("glad v0.1.36", "glfw 3.4", "glm 1.0.1", "spdlog v1.9.0", "magic_enum v0.9.5")

local outputdir = "$(mode)-$(os)-$(arch)"

-- A rule to copy resources from a target's 'Resources' directory.
rule("cp-resources")
  after_build(function (target)
    os.cp(target:name() .. "/Resources", "build/" .. outputdir .. "/" .. target:name() .. "/bin")
    os.cp(target:name() .. "/Shaders", "build/" .. outputdir .. "/" .. target:name() .. "/bin")
  end)

target("FlashlightEngine")
  -- Build as a static library
  set_kind("static")

  -- Set binary and object files directories.
  set_targetdir("build/" .. outputdir .. "/FlashlightEngine/bin")
  set_objectdir("build/" .. outputdir .. "/FlashlightEngine/obj")

  -- Set source cpp files.
  add_files("FlashlightEngine/Source/**.cpp")

  -- Add Engine headers to the project and set the include directory as public so it can be accessed from dependant
  -- targets.
  add_headerfiles("FlashlightEngine/Include/**.hpp", "FlashlightEngine/Include/**.h", "FlashlightEngine/Include/**.inl")
  add_includedirs("FlashlightEngine/Include", {public = true})

  -- Precompiled header
  set_pcxxheader("FlashlightEngine/Include/FlashlightEngine/pch.hpp")

  -- public dependencies
  add_packages("glad", "glfw", "glm", "spdlog", {public = true})
  -- private dependencies
  add_packages("magic_enum")

target("TestApplication")
    set_kind("binary")
    -- add_rules("cp-resources")

    set_targetdir("build/" .. outputdir .. "/TestApplication/bin")
    set_objectdir("build/" .. outputdir .. "/TestApplication/obj")

    add_files("TestApplication/Source/**.cpp")
    add_headerfiles("TestApplication/Include/**.hpp", "TestApplication/Include/**.h", "TestApplication/Include/**.inl")
    add_includedirs("TestApplication/Include")
    
    add_headerfiles("TestApplication/Shaders/**") -- A little trick to make them show in the VS/Rider solution.
    
    add_deps("FlashlightEngine")