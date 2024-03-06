set_xmakever("2.8.8")

set_project("Engine")
set_version("1.0.0")

set_allowedplats("windows", "linux")
set_allowedarchs("windows|x64", "linux|x64", "linux|x86_64")

add_rules("mode.debug", "mode.release")
set_languages("cxx20")

if (is_mode("release")) then
  set_optimize("fastest")
end

add_requires("glfw", "glm", "glad")

local outputdir = "$(mode)-$(os)-$(arch)"

rule("cp-resources")
  after_build(function (target)
    os.cp(target:name() .. "/Resources", "build/" .. outputdir .. "/" .. target:name() .. "/bin")
  end)

target("Engine")
  set_kind("binary")
  
  set_targetdir("build/" .. outputdir .. "/Engine/bin")
  set_objectdir("build/" .. outputdir .. "/Engine/obj")

  add_files("Engine/Source/**.cpp")
  add_headerfiles("Engine/Include/**.hpp", "Engine/Include/**.h", "Engine/Include/**.tpp")
  add_includedirs("Engine/Include", {public = true})

  add_packages("glfw")
  add_packages("glad")
  add_packages("glm")
