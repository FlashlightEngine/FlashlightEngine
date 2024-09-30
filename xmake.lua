set_xmakever("2.8.3")

set_project("FlashlightEngine")
set_license("Apache-2.0")

includes("xmake/**.lua")

option("static", {description = "Build the engine as a static library.", default = false})
option("override_runtime", {description = "Override VS runtime to MD in release and MDd in debug.", default = true})
option("enable_asserts", {description = "Enable assertions.", default = true})

local logger_options = {
  warn = "warnings",
  info = "info",
  debug = "debug",
  trace = "trace"
}

for opt, level in table.orderpairs(logger_options) do
  if level == "debug" or level == "trace" then
    option("enable_log_" .. opt, {description = "Enable " .. level .. " messages from the logger (only in debug).", default = true})
    
    if has_config("enable_log_" .. opt) and is_mode("debug") then
      add_defines("FL_LOG_" .. opt:upper() .. "_ENABLED")
    end
  else
    option("enable_log_" .. opt, {description = "Enable " .. level .. " messages from the logger.", default = true})

    if has_config("enable_log_" .. opt) then
      add_defines("FL_LOG_" .. opt:upper() .. "_ENABLED")
    end
  end
end

add_requires("vulkan-loader")

if is_plat("linux") then 
  add_requires("libx11")
  add_requires("libxkbcommon", {configs = {x11 = true}})
end 

-- Don't link system-installed libraries in CI
if os.getenv("CI") then
  add_requireconfs("*", {system = false})
  add_requireconfs("freetype.*", {system = false})
end

-- Global configuration
add_rules("mode.coverage", "mode.debug", "mode.releasedbg", "mode.release")
add_rules("plugin.vsxmake.autoupdate")

set_allowedplats("windows", "mingw", "linux")
set_allowedmodes("coverage", "debug", "releasedbg", "release")

add_includedirs("Include")

set_encodings("utf-8")
set_exceptions("cxx")
set_languages("clatest")
set_rundir("./bin/$(plat)_$(arch)_$(mode)")
set_targetdir("./bin/$(plat)_$(arch)_$(mode)")
set_warnings("allextra")

if is_mode("debug") then
  add_defines("FL_DEBUG")
elseif is_mode("coverage") then
  if not is_plat("windows") then
    add_links("gcov")
  end
elseif is_mode("releasedbg", "release") then
  set_fpmodels("fast")
  add_vectorexts("sse", "sse2", "sse3", "ssse3")
end

if not is_mode("release") then
  set_symbols("debug", "hidden")
end

add_cxflags("-Wno-missing-field-initializers", {tools = {"clang", "gcc"}})

if is_plat("windows") then
  if has_config("override_runtime") then
    set_runtimes(is_mode("debug") and "MDd" or "MD")
  end
elseif is_plat("mingw") then
  if is_mode("debug") then
    add_cxflags("-Og")
  end
  add_cxflags("-Wa,-mbig-obj")
end

if has_config("enable_assertions") then 
  add_defines("Fl_ASSERTIONS_ENABLED")
end

target("FlashlightEngine")
  if has_config("static") then
    set_kind("static")
    add_defines("FL_STATIC", {public = true})
  else
    set_kind("shared")
  end

  add_defines("FL_BUILD")

  -- Add header and source files
  for _, ext in ipairs({".h",  ".inl"}) do
    add_headerfiles("Include/(FlashlightEngine/**" .. ext .. ")")
    add_headerfiles("Source/FlashlightEngine/**" .. ext, {prefixdir = "private", install = false})
  end

  add_files("Source/FlashlightEngine/**.c")

  add_includedirs("Source", {private = true})
  add_rpathdirs("$ORIGIN")

  add_packages("vulkan-loader", {public = true})

  if is_plat("windows") then
    add_syslinks("User32")
  end

  if is_plat("linux") then
    add_packages("libx11", "libxkbcommon")
  end

includes("TestBed/xmake.lua")
