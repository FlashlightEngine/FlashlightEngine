set_xmakever("2.8.8")

set_project("FlashlightEngine")
set_version("1.0.0")

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

add_requires("glfw", "glm", "volk")

local outputdir = "$(mode)-$(os)-$(arch)"

rule("cp-resources")
  after_build(function (target)
    os.cp(target:name() .. "/Resources", "build/" .. outputdir .. "/" .. target:name() .. "/bin")
  end)

target("FlashlightEngine")
  set_kind("static")
  
  set_targetdir("build/" .. outputdir .. "/FlashlightEngine/bin")
  set_objectdir("build/" .. outputdir .. "/FlashlightEngine/obj")

  add_files("FlashlightEngine/Source/**.cpp")
  add_headerfiles("FlashlightEngine/Include/**.hpp", "FlashlightEngine/Include/**.h", "FlashlightEngine/Include/**.inl")
  add_includedirs("FlashlightEngine/Include", {public = true})
  set_pcxxheader("FlashlightEngine/Include/FlashlightEngine/pch.hpp")

  add_packages("glfw", "volk", "glm")

  add_defines("VK_NO_PROTOTYPES", {public = true})

target("TestApplication")
    set_kind("binary")

    set_targetdir("build/" .. outputdir .. "/TestApplication/bin")
    set_objectdir("build/" .. outputdir .. "/TestApplication/obj")

    add_files("TestApplication/Source/**.cpp")
    add_headerfiles("TestApplication/Include/**.hpp", "TestApplication/Include/**.h", "TestApplication/Include/**.inl")
    add_includedirs("TestApplication/Include")

    add_packages("glfw", "glm", "volk")

    add_deps("FlashlightEngine")